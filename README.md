# cpp-stl-loader
A simple program for reading STL 3D models

I wrote this for a speed comparison between Java, Go, and C++. Each one has a basic and smart STL loader. The basic simply parses the file and stores it as is: a list of faces each containing a normal and three points in space. The smart loader keeps the points in a separate array and has the faces point to them to ensure uniqueness. 

Here are some results for reading in an stl file with 295,645 faces and 149,281 unique vertices:

| Language | Basic Parse (ms) | Smart Parse (ms) | Repo |
| --- | --- | --- | --- |
| C++ | 116 | 372 | [here](https://github.com/jamethy/cpp-stl-loader) |
| Go | 758 | 1,535 | [here](https://github.com/jamethy/go-stl-loader) |
| Java | 6,390 | 6,670 | private |
