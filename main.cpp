/**
Pedro Martins, FCT-UNL, 2018

Aplication features:
1 - minMax rescaling
2 - mean difference
3 - standardisation
**/

#include "RunningStats.h"
#include "opencv2/core.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#define argi(name) cmd.get<int>(name)
using namespace std;
using namespace cv;

//input vector to be processed
const char *feature_vector = "";
//file to save result in CSV format
string outputPath ="" ;
//file to save maxmin data and meanStd in CSV format
string maxMinOutputPath ="" ;
string meanStdOutputPath ="" ;

int type = 0;
bool standard = false;

int total_items, numberOfFeatures;

vector< vector<double> > featureData;
vector< pair< double, double > > featureMaxMin;
vector< pair< double, double > > featureMeanStd;
vector< vector<double> > featureNormalisedData;

RunningStats runstats;

///methods
string printHelp();
void countData();
void loadData();
void computeMaxMin();
void rescale();
void computeMeanStd();
void standardize();
void meansub();
void saveData();
void saveMaxMinData();
void saveMeanStdData();


int main(int argc, const char **argv)
{

    auto start = chrono::high_resolution_clock::now();
    cout << "OpenCV norm tool.\n";
    try
    {
        const char *keys =
            "{ t | | }"
            "{ h | | }";
        CommandLineParser cmd(argc, argv, keys);

        ///parse the arguments

        if(cmd.has("h") || argc < 2)
        {
            cout << printHelp()<<endl;
            return 0;
        }
        if(cmd.has("t")) type = argi("t");

        feature_vector = argv[1];

        ///setup output files for norm and minmax values
        string tempName = feature_vector;
        string destName = tempName.substr(tempName.find_last_of('/') + 1, tempName.size());
        outputPath = destName.substr(0, destName.find_last_of('.'))+"_norm.csv";
        maxMinOutputPath = destName.substr(0, destName.find_last_of('.'))+"_maxmin.csv";
        meanStdOutputPath = destName.substr(0, destName.find_last_of('.'))+"_meanstd.csv";

        countData();

        cout << total_items <<" samples, "<< numberOfFeatures <<" features, "<<" type: "<<type<<endl;

        ///Initialize vectors
        featureData.assign(total_items, vector<double>(numberOfFeatures,0));
        featureNormalisedData.assign(total_items, vector<double>(numberOfFeatures,0));
        featureMaxMin.assign(numberOfFeatures, pair<double, double>(0.0,1000.0));
        featureMeanStd.assign(numberOfFeatures, pair<double, double>(0.0,0.0));

        loadData();         //1 - load feature vector from CSV file

        if(type==2)
        {
            computeMeanStd();
            saveMeanStdData();
            standardize();
        }
        else if(type==1)
        {
            computeMaxMin();
            saveMaxMinData();
            computeMeanStd();
            saveMeanStdData();
            meansub();
        }
        else
        {
            computeMaxMin();
            saveMaxMinData();
            rescale();
        }
        saveData();         //4 - save normalised vector to CSV
    }
    catch (const exception &e)
    {
        cout << "error: " << e.what() << endl;
        return -1;
    }

    auto fim = chrono::high_resolution_clock::now();
    cout << "processed in: " << std::chrono::duration_cast<chrono::milliseconds>(fim - start).count()
         << " ms" << endl;

    return 0;

}

void countData()
{
/// Count features and samples //////////////////////////////////////////////////////
    ifstream inputFeatureCountData(feature_vector);
    string current_line;
    int sampleCount = 0;     //number of samples
    int featureCount = 0;    //number of features

    // Start reading lines as long as there are lines in the file
    while(getline(inputFeatureCountData, current_line))
    {
        stringstream temp(current_line);
        string single_value;

        if (sampleCount==0)
        {
            while(getline(temp,single_value,','))
                featureCount++;
        }
        sampleCount++;
    }
    inputFeatureCountData.close();
    total_items = sampleCount;
    numberOfFeatures = featureCount;
}

void loadData()
{
    /// Set up features data //////////////////////////////////////////////////////
    ifstream inputFeatureData(feature_vector);
    string current_training_line;

    int y = 0;     //sample index

    // Start reading lines as long as there are lines in the file
    while(getline(inputFeatureData, current_training_line))
    {
        stringstream temp(current_training_line);
        string single_value;
        int z = 0;
        vector< double > tempVector(numberOfFeatures,0);

        while(getline(temp,single_value,','))
        {
            // convert the string element to a float value
            tempVector[z] = atof(single_value.c_str());
            z++;
        }
        featureData.at(y) = tempVector;
        y++;
    }
    inputFeatureData.close();
}

void computeMaxMin()
{
    for (int y = 0; y < total_items; y++)
    {
        for (int x = 0; x < numberOfFeatures; x++)
        {
            double cellValue = featureData.at(y).at(x);

            if( cellValue > featureMaxMin.at(x).first)
                featureMaxMin.at(x).first = cellValue;

            if(cellValue < featureMaxMin.at(x).second)
                featureMaxMin.at(x).second = cellValue;
        }
    }
}

void saveMaxMinData()
{

    ofstream evalBinData (maxMinOutputPath.c_str());

    for (int y = 0; y < 2; y++)
    {
        for (int x = 0; x < numberOfFeatures; x++)
        {
            if(y==0)
                evalBinData  << (double)featureMaxMin.at(x).first << ",";
            else  evalBinData  << (double)featureMaxMin.at(x).second << ",";
        }
        evalBinData  << "\n";
    }
    evalBinData.close();
    cout << "\nfinal maxmin data saved to file: " << maxMinOutputPath << endl;
}

void saveMeanStdData()
{

    ofstream evalBinData (meanStdOutputPath.c_str());

    for (int y = 0; y < 2; y++)
    {
        for (int x = 0; x < numberOfFeatures; x++)
        {
            if(y==0)
                evalBinData  << (double)featureMeanStd.at(x).first << ",";
            else  evalBinData  << (double)featureMeanStd.at(x).second << ",";
        }
        evalBinData  << "\n";
    }
    evalBinData.close();
    cout << "\nfinal meanStd data saved to file: " << meanStdOutputPath << endl;
}


void rescale()
{
    for (int y = 0; y < total_items; y++)
    {
        for (int x = 0; x < numberOfFeatures; x++)
        {
            double fMax = featureMaxMin.at(x).first;
            double fMin = featureMaxMin.at(x).second;
            double cValue = featureData.at(y).at(x);
            double um = (fMax-fMin);
            double dois = (cValue-fMin);
            double cNorm = (double)dois/um;
            featureNormalisedData.at(y).at(x) = cNorm;
        }
    }
}

void saveData()
{
    ofstream evalBinData (outputPath.c_str());

    for (int y = 0; y < total_items; y++)
    {
        for (int x = 0; x < numberOfFeatures; x++)
        {
            if(x!= numberOfFeatures-1)
                evalBinData  << (double)featureNormalisedData.at(y).at(x) << ",";
            else evalBinData  << (double)featureNormalisedData.at(y).at(x) << "\n";
        }
    }
    evalBinData.close();
    cout << "\nfinal normalised data saved to file: " << outputPath << endl;
}

void computeMeanStd()
{

    for (int y = 0; y < numberOfFeatures; y++)
    {
        for (int x = 0; x < total_items; x++)
        {
            double cellValue = featureData.at(x).at(y);
            runstats.Push(cellValue);
            //	cout << featureMaxMin.at(y).first<<" "<< featureMaxMin.at(y).second<<endl;
        }
        featureMeanStd.at(y).first = runstats.Mean();
        featureMeanStd.at(y).second = runstats.StandardDeviation();
        runstats.Clear();
    }
}
void standardize()
{
    for (int y = 0; y < total_items; y++)
    {
        for (int x = 0; x < numberOfFeatures; x++)
        {
            double fMean = featureMeanStd.at(x).first;
            double fStd = featureMeanStd.at(x).second;
            double cValue = featureData.at(y).at(x);
            double um = (fStd);
            double dois = (cValue-fMean);
            double cStandard = dois/um;
            featureNormalisedData.at(y).at(x) = cStandard;
        }
    }
}

void meansub()
{
    for (int y = 0; y < total_items; y++)
    {
        for (int x = 0; x < numberOfFeatures; x++)
        {
            double fMax = featureMaxMin.at(x).first;
            double fMin = featureMaxMin.at(x).second;
            double fMean = featureMeanStd.at(x).first;
            double cValue = featureData.at(y).at(x);
            double um = (fMax-fMin);
            double dois = (cValue-fMean);
            double cMeansub = dois/um;
            featureNormalisedData.at(y).at(x) = cMeansub;
        }
    }
}

string printHelp()
{
    string h ="Help\n"
              "Usage: tool-norm <inputFile> [arguments]\n\n"
              "Arguments:\n"
              "  -t, type of normalisation(default=0): -t=0, scaling\n"
              "      -t=1, mean subtraction and -t=2, standardisation\n"
              "  -h, this help message.\n";

    return h;
}
