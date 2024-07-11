# AMX Trace Generator and Macsim Integration

This guide provides detailed instructions on how to compile, run, and integrate the AMX trace generator with Macsim.

## Step-by-Step Instructions

### 1. Compiling the AMX Trace Generator

To generate traces, you need to compile the `amx_trace_generator.cpp` with your desired matrix parameters.

#### Example Compilation Command

Use the following command to compile the `amx_trace_generator.cpp` file:

```sh
g++ -o amx_trace_generator amx_trace_generator.cpp -lz
```

#### Setting Matrix Parameters

Before compiling, ensure that the matrix parameters (`MATRIX_A_ROWS`, `MATRIX_A_COLS`, `MATRIX_B_ROWS`, `MATRIX_B_COLS`) are set correctly. You can modify these parameters in the source code or pass them as command-line arguments when running the compiled program. 
./amx_trace_generator <MATRIX_A_ROWS> <MATRIX_A_COLS> <MATRIX_B_ROWS> <MATRIX_B_COLS>

### 2. Running the AMX Trace Generator

After compiling, run the executable to generate the trace files.

#### Example Run Command

```sh
./amx_trace_generator 16 64 64 64
```

This will produce:
- `output.1_0.raw`: The Macsim executable trace file.
- `output.1.txt`: The human-readable trace file.

### 3. Moving Trace Files to SST Unit Test Folder

After generating the trace files, move them to the `sst-unit-test` folder.

#### Example Move Command

```sh
mv output.1_0.raw output.1.txt /path/to/sst-unit-test/
```

Replace `/path/to/sst-unit-test/` with the actual path to your `sst-unit-test` folder.

### 4. Configuring and Running Macsim

Update the `bin/trace_file_list` file in your Macsim directory to point to the `.txt` file.

#### Example `trace_file_list` Entry

```txt
1
/path/to/sst-unit-test/output.1.txt
```

Replace `/path/to/sst-unit-test/` with the actual path where the `output.1.txt` file is located.

#### Running Macsim

Run Macsim normally with the updated trace file list.

#### Example Macsim Run Command

```sh
./macsim
```
