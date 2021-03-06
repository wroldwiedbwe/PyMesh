/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <MeshUtils/LongEdgeRemoval.h>
#include <TestBase.h>

class LongEdgeRemovalTest : public TestBase {
    protected:
        void ASSERT_NO_LONG_EDGES(
                const MatrixFr& vertices,
                const MatrixIr& faces,
                Float max_length) {
            const size_t num_faces = faces.rows();
            const size_t vertex_per_face = faces.cols();
            for (size_t i=0; i<num_faces; i++) {
                const auto& f = faces.row(i);
                for (size_t j=0; j<vertex_per_face; j++) {
                    const auto& curr_v = vertices.row(f[j]);
                    const auto& next_v = vertices.row(f[(j+1)%vertex_per_face]);
                    ASSERT_LT((curr_v - next_v).norm(), max_length);
                }
            }
        }
};

TEST_F(LongEdgeRemovalTest, SingleTraingle) {
    MatrixFr vertices(3,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0;
    MatrixIr faces(1,3);
    faces << 0,1,2;

    Float max_length = 1.1;
    LongEdgeRemoval remover(vertices, faces);
    remover.run(max_length);

    auto out_vertices = remover.get_vertices();
    auto out_faces = remover.get_faces();

    ASSERT_EQ(4, out_vertices.rows());
    ASSERT_EQ(2, out_faces.rows());
    ASSERT_NO_LONG_EDGES(out_vertices, out_faces, max_length);
}

TEST_F(LongEdgeRemovalTest, SingleLargeTraingle) {
    MatrixFr vertices(3,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0;
    MatrixIr faces(1,3);
    faces << 0,1,2;

    Float max_length = 0.1;
    LongEdgeRemoval remover(vertices, faces);
    remover.run(max_length);

    auto out_vertices = remover.get_vertices();
    auto out_faces = remover.get_faces();
    write_mesh_raw("long_edge_test.obj", out_vertices, out_faces);

    ASSERT_NO_LONG_EDGES(out_vertices, out_faces, max_length);
}

TEST_F(LongEdgeRemovalTest, triangle_2) {
    MatrixFr vertices(3,3);
    vertices <<
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.5, 0.0;
    MatrixIr faces(1,3);
    faces << 0,1,2;

    Float max_length = 0.9;
    LongEdgeRemoval remover(vertices, faces);
    remover.run(max_length);

    auto out_vertices = remover.get_vertices();
    auto out_faces = remover.get_faces();

    ASSERT_NO_LONG_EDGES(out_vertices, out_faces, max_length);
}

TEST_F(LongEdgeRemovalTest, ori_face) {
    MatrixFr vertices(4, 2);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                0.0, 1.0,
                1.0, 1.0;
    MatrixIr faces(2, 3);
    faces << 0, 1, 2,
             2, 1, 3;

    Float max_length = 0.1;
    LongEdgeRemoval remover(vertices, faces);
    remover.run(max_length);

    auto out_vertices = remover.get_vertices();
    auto out_faces = remover.get_faces();
    const auto ori_faces = remover.get_ori_faces();

    ASSERT_NO_LONG_EDGES(out_vertices, out_faces, max_length);

    const size_t num_faces = out_faces.rows();
    ASSERT_EQ(num_faces, ori_faces.size());
    for (size_t i=0; i<num_faces; i++) {
        const Vector3I f = out_faces.row(i);
        const Vector2F c = (out_vertices.row(f[0]) +
                out_vertices.row(f[1]) +
                out_vertices.row(f[2])) / 3.0;
        std::cout << c[0] << ", " << c[1] << std::endl;
        std::cout << ori_faces[i] << std::endl;
        if (c[0] + c[1] < 1.0) {
            ASSERT_EQ(0, ori_faces[i]);
        } else {
            ASSERT_EQ(1, ori_faces[i]);
        }
    }
}

