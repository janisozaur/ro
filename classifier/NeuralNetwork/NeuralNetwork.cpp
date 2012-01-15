#include "NeuralNetwork.h"
#include "NetworkLayer.h"
#include "Connection.h"
#include "../../common/TypeCommon.h"
#include "../../common/CommonDefines.h"

#include <cmath>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <limits>
#ifdef HAS_ELAPSED_TIMER
    #include <QElapsedTimer>
#else
    #include <QTime>
#endif

#define TEACH_BIAS_MOMENTUM

NeuralNetwork::NeuralNetwork()
{
    id = qrand();
}


QVector<nnreal> NeuralNetwork::classifyVec(const QVector<nnreal> &input)
{
    for (int i = 0; i < mLayers.size(); i++) {
        if (i == 0) {
            for (int j = 0; j < mLayers.at(0)->neuronsCount(); j++) {
                mLayers.at(0)->neuron(j)->updateOutput(input[j]);
            }
        } else {
            mLayers.at(i)->calculate();
        }
    }

    QVector<nnreal> result(mLayers.at(mLayers.size() - 1)->neuronsCount());
    for (int i = 0; i < result.size(); i++) {
        result[i] = mLayers.at(mLayers.size() - 1)->neuron(i)->output();
    }
    return result;
}

nnreal NeuralNetwork::backprop(const QVector<nnreal> &input,
                               const QVector<nnreal> &targetOutput,
                               const nnreal &learningRate,
                               const nnreal &momentum)
{
    const int lastLayerIdx = mLayers.size() - 1;
    const int lastLayerNeuronsCount = mLayers.at(lastLayerIdx)->neuronsCount();

    QVector<nnreal> output = classifyVec(input);

    QVector<nnreal> lastLayerError;
    lastLayerError.resize(lastLayerNeuronsCount);

    NetworkLayer *prevLayer = mLayers.at(lastLayerIdx - 1);

    QVector<nnreal> previousLayerError(prevLayer->neuronsCount());

    nnreal averageError = 0.0;

    for (int i = 0; i < lastLayerNeuronsCount; i++) {
        Neuron *neuron = mLayers.at(lastLayerIdx)->neuron(i);

        const nnreal errorTemp = targetOutput.at(i) - output.at(i);
        neuron->updateDelta(errorTemp);

        averageError += errorTemp * errorTemp;

        const int connCount = neuron->connectionsCount();
        const nnreal delta = neuron->delta();
        for (int j = 0; j < connCount; j++) {
            Connection *conn = neuron->connection(j);
            const nnreal weight = conn->weight();
            const int endNeuron = conn->neuronIndex();
            previousLayerError[endNeuron] += delta * weight;

            const nnreal momentumCoef = momentum * conn->weightDelta();
            const nnreal prevNeuronOut = prevLayer->neuron(endNeuron)->output();
            const nnreal lrCoef = prevNeuronOut * delta * learningRate;
            const nnreal weightDelta = momentumCoef + lrCoef;
            conn->setWeightDelta(weightDelta);
            conn->setWeight(weight + weightDelta);
        }

        nnreal momentumCoef = 0;
#ifdef TEACH_BIAS_MOMENTUM
        momentumCoef = momentum * neuron->biasDelta();
#endif
        const nnreal lrCoef = learningRate * delta;
        const nnreal weightDelta = momentumCoef + lrCoef;
        neuron->setBiasDelta(weightDelta);
        neuron->setBias(neuron->bias() + weightDelta);
    }

    for (int i = lastLayerIdx - 1; i > 0; i--) {
        previousLayerError = mLayers.at(i)->backprop(previousLayerError,
                                                     learningRate, momentum);
    }

   // qDebug()<<output<<targetOutput<<averageError;
   // qDebug()<<fabs(averageError / (float)lastLayerNeuronsCount);
    return fabs(averageError) / (float)lastLayerNeuronsCount;
}


void NeuralNetwork::addLayer(const int &numNeurons,
                             const Neuron::ActivationType &activationFunctionType)
{
    NetworkLayer *newLayer;
    if (!mLayers.isEmpty()) {
        newLayer = new NetworkLayer(mLayers.last(), activationFunctionType);
    } else {
        newLayer = new NetworkLayer(NULL, activationFunctionType);
    }

    mLayers.append(newLayer);
    newLayer->addNeurons(numNeurons);
}

quint8 NeuralNetwork::classify(const QVector<nnreal> &input)
{
    QVector<nnreal> output = classifyVec(input);

    return outputVectorToLabel(output);
}

quint8 NeuralNetwork::outputVectorToLabel(const QVector<nnreal> &output) const
{
    int maxIndex = 0;
    nnreal max = -std::numeric_limits<nnreal>::infinity();

    for (int i = 0; i < output.size(); i++) {
        if (output.at(i) > max) {
            max = output.at(i);
            maxIndex = i;
        }
    }
    quint8 result;
    switch (maxIndex) {
        default:
        case 0:
            result = 32;
            break;
        case 1:
            result = 96;
            break;
        case 2:
            result = 160;
            break;
        case 3:
            result = 224;
            break;
    }
    return result;
}

QVector<float> NeuralNetwork::train(const QVector<QVector<nnreal> > &train,
                          const QVector<QVector<nnreal> > &exOutput,
                          const int &maxEpochs, const nnreal &learningRate,
                          const nnreal &momentum, const nnreal &desiredError,
                          QDataStream &nnout)
{
    QTextStream out(stdout);
    nnreal epochError = std::numeric_limits<nnreal>::infinity();
    int epochNum = 0;
    double lr = learningRate;
    QList<int> indicesBase;
    QVector<float> result;
#ifdef HAS_VECTOR_RESERVE
    indicesBase.reserve(train.size());
    result.reserve(maxEpochs);
#endif
    for (int i = 0; i < train.size(); i++) {
        indicesBase.append(i);
    }
#ifdef HAS_ELAPSED_TIMER
    QElapsedTimer epochTimer;
#else
    QTime epochTimer;
#endif
    epochTimer.start();
    float best = std::numeric_limits<nnreal>::infinity();
    while (epochError > desiredError && epochNum < maxEpochs) {

        epochError = 0;

        QList<int> indices = indicesBase;

        int sampleNum = 0;
        while (!indices.empty()) {
            const int index = qrand() % indices.size();
            const int i = indices.takeAt(index);
            const QVector<nnreal> input = train.at(i);
            const QVector<nnreal> output = exOutput.at(i);
            sampleNum++;

            //qDebug()<<output;

            epochError += backprop(input, output, lr, momentum);

            //out<<"Epoch: "<<epochNum<<" progress: "<<(int)(100*(float)sampleNum/trainingSetSize)<<"%";
            //if(!indices.empty())out<<"\r";
        }

        epochError /= train.size();
        bool saved = false;
        if (epochError < best) {
            best = epochError;
            nnout << *this;
            saved = true;
        } else if (epochError > best * 3 / 2) {
            epochNum = maxEpochs;
        }
        result << epochError;

        lr *= 0.999;
        const int msecs = epochTimer.restart();
        out << "Epoch: " << epochNum << " error: " << epochError << ", "
            << msecs << "msecs, saved: " << saved << endl;
        epochNum++;
    }
    return result;
}

QDataStream &operator<<(QDataStream &stream, const NeuralNetwork &nn)
{
	int size = sizeof(nnreal);
	qDebug() << "writing size:" << size;
	stream << size;
	stream << nn.mLayers.size();

	for (int i = 0; i < nn.mLayers.size(); i++) {
		stream << nn.mLayers.at(i)->neuronsCount();
		for (int j = 0; j < nn.mLayers.at(i)->neuronsCount(); j++) {
			Neuron *neuron = nn.mLayers.at(i)->neuron(j);
			stream << neuron->bias();
			stream << int(neuron->activationType());
			stream << neuron->connectionsCount();

			for (int z = 0; z < neuron->connectionsCount(); z++) {
				Connection *conn = neuron->connection(z);
				stream << conn->neuronIndex();
				stream << conn->weight();
				stream << conn->weightDelta();
			}
		}
	}
	return stream;
}

QTextStream &operator<<(QTextStream &stream, const NeuralNetwork &nn)
{
	int size = sizeof(nnreal);
	stream << "size " << size << endl;
	stream << "number of layers " << nn.mLayers.size() << endl;

	for (int i = 0; i < nn.mLayers.size(); i++) {
		stream << "layer " << i << " neuron count "
			   << nn.mLayers.at(i)->neuronsCount() << endl;
		for (int j = 0; j < nn.mLayers.at(i)->neuronsCount(); j++) {
			Neuron *neuron = nn.mLayers.at(i)->neuron(j);
			stream << "neuron " << j << endl;
			stream << "bias " << neuron->bias() << " activation "
				   << int(neuron->activationType()) << " connections_count "
				   << neuron->connectionsCount() << endl;

			for (int z = 0; z < neuron->connectionsCount(); z++) {
				Connection *conn = neuron->connection(z);
				stream << "conn " << z << " index " << conn->neuronIndex()
					   << " weight " << conn->weight() << " wd "
					   << conn->weightDelta() << endl;
			}
		}
	}
	return stream;
}


QDataStream &operator>>(QDataStream &stream, NeuralNetwork &nn)
{
    int size;
    stream >> size;
    Q_ASSERT_X(size == sizeof(nnreal), __PRETTY_FUNCTION__, "Sorry, the network you're trying to load has invalid nnreal size");

    nn.mLayers.clear();

    int layersNum;
    stream >> layersNum;

    for (int i = 0; i < layersNum; i++) {
        int neuronNum;
        stream >> neuronNum;

        nn.addLayer(neuronNum);

        for (int j = 0; j < neuronNum; j++) {
            Neuron *neuron = nn.mLayers.at(i)->neuron(j);

            nnreal bias;
            stream >> bias;
            neuron->setBias(bias);

            Neuron::ActivationType at;
            int atInt;
            stream >> atInt;
            at = Neuron::ActivationType(atInt);
            neuron->setActivationType(at);

            int connectionsNum;
            stream >> connectionsNum;
            for (int k = 0; k < connectionsNum; k++) {
                int neuronIndex;
                nnreal weight;
                nnreal weightDelta;

                stream >> neuronIndex;
                stream >> weight;
                stream >> weightDelta;

                Connection *conn = neuron->connection(k);
                conn->setNeuronIndex(neuronIndex);
                conn->setWeightDelta(weightDelta);
                conn->setWeight(weight);
            }
        }
    }
    return stream;
}
