# OCR-Lezcollitade

## The CLI

### Subcommands

#### train

- Loading network.

> `--generate <string>`

Generates a neural network based on string. 
The string should be in the form of "<input-neurons>,<neurons-layer-1>,...,<output-neurons>".

*Example: `Lezcollitade train --generate "2,2,1"`* generates a network with 2 inputs, 1 hidden layer of 2 neurons, and 1 output neuron.

> `--input-network|-i <file>`

Loads a network from a file. If the file doesn't exist, an error will be raised.

*NB: Either `--generate` or `--input-network` are required.*

> `--output-activation <sigmoid|softmax> (default: sigmoid)`

The default activation function for the output layer.

> `--activation <sigmoid|softmax> (default: sigmoid)`

The default activation function for the hidden layers.

*NB: The activation functions are saved within the network, thus only valid
when `--generate` is used. Otherwise, the functions will be loaded from
the network file.*

- Output.

> `--output-network|-o <file>`

Once the training is done, outputs the network to given file, and creates it if the file does not exist.

- Data.

> `--data <file> (required)`

Loads data from a file based on the format. If the file does not exist, an error is raised.

> `--format <csv|bin> (required)`

The format of the data.

The `csv` format should be written as each line containing an entry, and
containing as first value the index of the neuron expected to be fired
(-1 if none), and the value of each input from 0 to 255 included.

*Example: xor training data*
```CSV
-1,0,0
-1,1,1
0,1,0
0,0,1
```
The bin format is as of now not functional...

> `--length <int> (default: -1)`

The number of line to be read in the file. If not specified (or -1),
all the file up to the end will be loaded.

> `--start <int> (default: 0)`

The number of lines the loader should start at. If none specified, lines will
be loaded from line 0.

- Training parameters.

> `--iterations <int>` 

The number of iterations to run the data set on the network. If none specified,
the training will go on until `sigstop` is raised.

> `--rate <float> (default: 0.1)`

The learning rate. If none specified, the default learning rate will apply.
*Warning: Using an oversized learning rate can cause double overflow thus
causing the neural network to contain `nan` values.*

> *Example:* `Lezcollitade train --generate "784,30,10" --output-network
> "./output.net" --data "./mnist_train_data.csv" --format csv --rate 0.01
> --start 1 --output-activation softmax --iterations 100`

Creates a new neural network with 784 inputs, 30 neurons in the hidden layer, 
and 10 outputs, saved in "./output.net" and trained 100 times on the data of
`./mnist_train_data`, except the first line (which is ususally the header
in a csv file) and whose output layer activaiton function is the `soft max` 
function.

> *Example:* `Lezcollitade train --input-network "./output.net"
> --output-network "./output.net" --data "./mnist_train_data.csv"
> --format csv --start 1 --iterations 10`

Loads the neural network in "./output.net", trains it 10 times
over all the lines of `./mnist_train_data.csv` except the first line, and
saves it back to "./output.net".

#### Test

> `--generate <string>`

> `--input-network|-i <file>`

*See Train*

> `--data <file>`

> `--format <csv|bin>`

*See Train*

> `--verbose|-v`
Indicates whether each tested input should be displayed along with its result
in the command line.

> *Example:* `Lezcollitade test --input-network "./output.net" --data
> "./mnist_test_data.csv" --format csv --start 1`

Tests the performances of the network contained in "./output.net"
over all the lines of `./mnist_test_data.csv` except the first line.

#### Convert

> `--input-network|-i <file>`
The input network to test on.

> `--grid <dir>`
The folder to the images of the grid.

> `--data|-d`
The path to the single image to be tested.

> `--mode <single|multi>`
Whether this tests with the `grid` folder or the `data` image path.
