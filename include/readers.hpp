#ifndef READER_HPP_
#define READER_HPP_

// Supported file formats
enum FileFormat {
    Ply,
    Step,
};

TriangularMesh read_ply_triangular_mesh(const string& path);
TriangularMesh read_step_triangular_mesh(const string& path);

TriangularMesh read_triangular_mesh(const string& path, FileFormat format);

#endif // READER_HPP_
