#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

using namespace std;

class Data
{
private: // for test
    static size_t new_cnt, delete_cnt;

public:
    int *entry;         // the starting address of entries
    size_t row, col;    // the shape of entries
    size_t ref_cnt;     // the count of matrices referring to this Data

    Data(size_t row, size_t col):
        row(row), col(col), ref_cnt(0)
    {
        new_cnt++;
        entry = new int[row * col]{};
    }

    ~Data()
    {
        delete_cnt++;
        delete[] entry;
    }

    static size_t check_cnt()
    { return new_cnt - delete_cnt; }
};
size_t Data::new_cnt = 0;
size_t Data::delete_cnt = 0;

class Matrix
{   
public:
    Data *data;         // the ptr pointing to the entries
    size_t start;       // the starting index of ROI
    size_t row, col;    // the shape of ROI

    Matrix():
        data(nullptr), start(0), row(0), col(0) {}

    ~Matrix()
    {
        if (!data)
            return;
        if (!--data->ref_cnt)
            delete data;
    }
};

void print_matrix(Matrix &mat)
{
    for (size_t r = 0; r < mat.row; r++)
    {
        size_t head = mat.start+r*mat.data->col;
        for (size_t c = 0; c < mat.col; c++)
            cout << mat.data->entry[head + c] << ' ';
        cout << '\n';
    }
    cout << endl;
}


void unload_data(Matrix &mat)
{
    if (!mat.data)
        return;
    if (!--mat.data->ref_cnt)
        delete mat.data;
    mat.data = nullptr;
    mat.start = mat.row = mat.col = 0;
}

void create_data(Matrix &mat, size_t row, size_t col)
{
    unload_data(mat);
    mat.data = new Data(row, col);
    mat.data->ref_cnt = 1;
    mat.start = 0, mat.row = row, mat.col = col;
}

void load_data(Matrix &mat, Data *data, size_t start, size_t row, size_t col)
{
    unload_data(mat);
    mat.data = data;
    data->ref_cnt++;
    mat.start = start;
    mat.row = row;
    mat.col = col;
}

void shallow_copy(Matrix &dest, Matrix &src)
{
    unload_data(dest);
    dest = src;
    src.data->ref_cnt++;
}

void deep_copy(Matrix &dest, Matrix &src)
{
    size_t row = src.row, col = src.col;
    create_data(dest, row, col);
    for (size_t r = 0; r < row; r++)
        memcpy(dest.data->entry + r * col, src.data->entry + src.start + r * src.data->col, col * sizeof(int));
}

bool equal_matrix(Matrix &a, Matrix &b)
{
    if (a.row != b.row || a.col != b.col)
        return false;
    size_t row = a.row, col = b.col;
    for (size_t r = 0; r < row; r++)
        for (size_t c = 0; c < col; c++)
            if (a.data->entry[a.start + r * a.data->col + c] != b.data->entry[b.start + r * b.data->col + c])
                return false;
    return true;
}

void add_matrix(Matrix &dest, Matrix &a, Matrix &b)
{
    size_t row = a.row, col = b.col;
    create_data(dest, row, col);
    for (size_t r = 0; r < row; r++)
        for (size_t c = 0; c < col; c++)
            dest.data->entry[r * col + c] = a.data->entry[a.start + r * a.data->col + c] + b.data->entry[b.start + r * b.data->col + c];
}

void minus_matrix(Matrix &dest, Matrix &a, Matrix &b)
{
    size_t row = a.row, col = b.col;
    create_data(dest, row, col);
    for (size_t r = 0; r < row; r++)
        for (size_t c = 0; c < col; c++)
            dest.data->entry[r * col + c] = a.data->entry[a.start + r * a.data->col + c] - b.data->entry[b.start + r * b.data->col + c];
}

void multiply_matrix(Matrix &dest, Matrix &a, Matrix &b)
{
    size_t row = a.row, col = b.col, K = a.col;
    create_data(dest, row, col);
    for (size_t i = 0; i < row; i++)
        for (size_t j = 0; j < col; j++)
            for (size_t k = 0; k < K; k++)
                dest.data->entry[i * col + j] += a.data->entry[a.start + i * a.data->col + k] * b.data->entry[b.start + k * b.data->col + j];
}
