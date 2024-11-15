# Content Based Shape Retrieval System (CBSR)

This project was done for the course Multimedia Retrieval at Utrecht University. 
We used (almost exlusively) the C++ language, except for some analysis scripts for which we used Python.

Libraries that we used are listed below.

For rendering: OpenGL, GLEW, GLFW
For mesh processing: VCG, CGAL

## Installation

The code should be run as a Visual Studio Code solution. We recommend the newest Visual Studio Code Version (2017), lower versions are not tested.

### Step 1
clone the repository with e.g.

```bash
    git clone https://github.com/MagnusMatre/MMR.git
```

or download the .zip contents of the main branch. Since we include the files of the meshes, convex hulls and sample points (~6 GB), this will take a while.

### Step 2
After cloning the repository, look for the "OpenGl.sln" file and open it in Visual Studio Code (2017).

### Step 3
You can now build the solution in x64 configuration. All external dependencies should also be downloaded in the "src/OpenGL/Dependencies" and "src/vendor" directories.
Building the solution may take 5-10 minutes.
*Important: * There are two possible build configurations (selected in the toolbar in the Visual Studio Code editor):

1. Debug. In this build configuration, you can test the actual CBSR application by selecting shapes. Moving around is done with WASD LSHIFT (down) SPACE (up), looking side-to side and up-down is done with the arrow keys, zooming is done with ZX. Toggling wireframe is done with M.
2. RunScript. In this configuration, you can run scripts to reproduce our results (see below).

### Step 4 (optional)

Python scripts can be run by going inside the "src/Python" folder in a terminal. We mostly used these files to create plots/analyze results.
Scripts can be run with ```bash
    python3 .\<script_file_name>.py
```

## Scripts

When running the code in the "RunScript" configuration, you need to supply command line arguments.
Command line arguments can be set by right clicking "OpenGL" in the Solution Explorer in Visual Studio Code.
Then, go to Properties -> Debug -> Debugging -> Command Arguments.

Below we list the possible command line arguments and which results they reproduce.

1. FullRemesh -- This script will clean and remesh the meshes in the "MeshesFiltered" directory and save them in the "MeshesFilteredRemeshed" directory. Note, this takes a few hours...
2. Normalize  -- This script will normalize all the meshes in the "MeshesFilteredRemeshed" and save them in "MeshesFilteredNormalized". This step takes around an hour.
3. HullsAndSamplePoints -- This script will create the convex hulls and sample points for all meshes in "MeshesFilteredNormalized". It is necessary to run BEFORE FeatureExtraction. This step also takes a few hours.
4. FeatureExtraction -- This script will extract the *RAW* features from "MeshesFilteredNormalized" directory. This step again takes a while.
                        * IMPORTANT : * After obtaining the feature files from each directory, you need to run the Python script
                        ```bash 
                            python3 modify_features.py
                        ```
                        This script will stitch together all the different feature files and add secondary features. The result is the "features_final.txt" file in the "res/" folder.

5. QueryExperiments -- This script will run all the experiments we did for different normalization
6. ScalabilityExperiments -- 
