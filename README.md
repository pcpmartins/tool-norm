# Tools for data normalisation

This tools are used during the data preprocessing step to standardize the range of independent variables or features of data. Many machine learning algorithms(namely SVM) rely on this preprocessing step to equalize the relevancy of features attributing them a common range of values. This step is essential to achieve a good classification performance, because when one of the features has a broad range of values, the overall result will be governed by this particular feature. Therefore, the range of all features should usually be normalized so that each feature contributes approximately proportionately to the final result.

## Development setup

* Ubuntu 16.04 LTS
* CodeBlocks 16.01
* OpenCV 3.2.0

## Aplication features

* 1 - minMax rescaling
* 2 - mean difference
* 3 - standardisation

![figure 1](/images/tool-norm-formulae-01.png)
*figure 1 - Available types of normalisation*

## Output

This application outputs diferently depending on the -t argument. In presence of an input file (e.g:input.csv) it will output:
* input_norm.csv - The normalised feature vector.
* input_maxmin.csv - The max and min values for each feature.(2rows x number of features size).
* input_meanstd.csv - The mean and std. variation values for each feature(2rows x number of features size).

## Usage

 tool-norm [inputFile] [arguments]

 ### Arguments:

* -t=0, scaling
* -t=1, mean subtraction(z-score)
* -t=2, standardisation
* -h, help message

### Example

Here goes a simple example on how to use this tool with the example file provided with this project(LBTraining.csv).

![figure 2](/images/tool-norm-example.png)
*figure 2 - Different outputs according to -t argument.*

On the first column we can see the content of the input file, on the next columns we can see the shell commands used for each feature scaling type together with the output files content for each command.