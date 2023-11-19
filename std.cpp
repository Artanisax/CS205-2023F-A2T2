//PREPEND BEGIN
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
//PREPEND END

//TEMPLATE BEGIN
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
//TEMPLATE END

//APPEND BEGIN
void t1()
{
    Data *data[4];
    for (size_t i = 0; i < 2; i++)
        data[i] = new Data(3, 2);
    for (size_t i = 0; i < 6; i++)
    {
        data[0]->entry[i] = i;
        data[1]->entry[i] = i / 2;
    }
    Matrix A, B, C;
    load_data(A, data[0], 0, 3, 2);
    load_data(B, data[1], 0, 3, 2);
    add_matrix(C, A, B);
    print_matrix(C);

    Matrix D, E, F;
    for (size_t i = 2; i < 4; i++)
        data[i] = new Data(1, 4);
    for (size_t i = 0; i < 4; i++)
    {
        data[2]->entry[i] = i + 1;
        data[3]->entry[i] = i * i;
    }
    load_data(D, data[2], 0, 1, 4);
    load_data(E, data[3], 0, 1, 4);
    minus_matrix(F, E, D);
    print_matrix(F);
}

void t2()
{
    Matrix mat[6];
    Data *data[4];
    for (size_t i = 0; i < 2; i++)
        data[i] = new Data(4, 4);
    data[2] = new Data(5, 3);
    data[3] = new Data(3, 5);
    for (size_t i = 0; i < 16; i++)
    {
        data[0]->entry[i] = i * i / 7;
        data[1]->entry[i] = i / 7 + 1;
    }
    for (size_t i = 0; i < 15; i++)
    {
        data[2]->entry[i] = i * 7 / (i - i / 2 + 1);
        data[3]->entry[i] = 2 - i % 7;
    }
    for (size_t i = 0; i < 4; i++)
        load_data(mat[i], data[i], 0, data[i]->row, data[i]->col);
    multiply_matrix(mat[4], mat[0], mat[1]);
    print_matrix(mat[4]);
    multiply_matrix(mat[4], mat[1], mat[0]);
    print_matrix(mat[4]);
    multiply_matrix(mat[5], mat[2], mat[3]);
    print_matrix(mat[5]);
    multiply_matrix(mat[5], mat[3], mat[2]);
    print_matrix(mat[5]);
}

void t3()
{
    Data *data[4] = {
        new Data(4, 6),
        new Data(3, 8),
        new Data(4, 9),
        new Data(4, 9),
    };
    Matrix mat[4];
    for (size_t i = 0; i < 24; i++)
        data[0]->entry[i] = data[1]->entry[i]  = i % 12;
    for (size_t i = 0; i < 36; i++)
        data[2]->entry[i] = data[3]->entry[i]  = (i % 7) / (7 / (i + 1) + 1);
    for (size_t i = 0; i < 4; i++)
        load_data(mat[i], data[i], 0, data[i]->row, data[i]->col);
    cout << equal_matrix(mat[0], mat[1]) << endl;
    cout << equal_matrix(mat[2], mat[3]) << endl;
}

void t4()
{
    Data *data[3] = {
        new Data(8, 6),
        new Data(6, 4),
        new Data(8, 4),
    };
    Matrix mat[5];
    for (size_t i = 0; i < 48; i++)
        data[0]->entry[i] = (i - 24) * i;
    for (size_t i = 0; i < 24; i++)
        data[1]->entry[i] = ((int)i - 12) * ((int)i + 12);
    for (size_t i = 0; i < 32; i++)
        data[2]->entry[i] = -i;
    for (size_t i = 0; i < 3; i++)
        load_data(mat[i], data[i], 0, data[i]->row, data[i]->col);
    multiply_matrix(mat[3], mat[0], mat[1]);
    print_matrix(mat[3]);
    minus_matrix(mat[4], mat[3], mat[2]);
    print_matrix(mat[4]);
    print_matrix(mat[0]);
    load_data(mat[4], data[0], 0, data[0]->row, data[0]->col);
    print_matrix(mat[4]);
    load_data(mat[4], data[1], 0, data[1]->row, data[1]->col);
    print_matrix(mat[4]);
    print_matrix(mat[0]);

    Matrix deep, shallow;
    unload_data(deep);
    unload_data(shallow);
    deep_copy(deep, mat[0]);
    shallow_copy(shallow, mat[0]);
    print_matrix(mat[0]);
    print_matrix(deep);
    print_matrix(shallow);

    mat[0].data->entry[0] = 2333;
    print_matrix(mat[0]);
    print_matrix(deep);
    print_matrix(shallow);
}

void t5()
{
    Data *data[2] = {
        new Data(8, 8),
        new Data(6, 6),
    };
    for (size_t i = 0; i < 64; i++)
        data[0]->entry[i] = i % 8 % 6;
    for (size_t i = 0; i < 36; i++)
        data[1]->entry[i] = i % 6;
    Matrix mat[4];
    load_data(mat[0], data[0], 1, 4, 3);
    load_data(mat[1], data[0], 33, 4, 3);
    load_data(mat[2], data[1], 1, 4, 3);
    load_data(mat[3], data[1], 0, 4, 3);
    print_matrix(mat[0]);
    print_matrix(mat[1]);
    print_matrix(mat[2]);
    print_matrix(mat[3]);
    cout << equal_matrix(mat[0], mat[1]) << '\n' <<
        equal_matrix(mat[1], mat[2]) << '\n' <<
        equal_matrix(mat[2], mat[3]) << '\n' <<
        endl;
}

void t6()
{
    Data *data[2] = {
        new Data(8, 10),
        new Data(12, 7),
    };
    Matrix mat[6];
    for (size_t i = 0; i < 80; i++)
        data[0]->entry[i] = 2 * i + 1;
    for (size_t i = 0; i < 84; i++)
        data[1]->entry[i] = (int)i - (int)(i - 1) / 2 + 1;
    load_data(mat[0], data[0], 1, 2, 7);
    load_data(mat[1], data[1], 14, 2, 7);
    print_matrix(mat[0]);
    print_matrix(mat[1]);

    add_matrix(mat[2], mat[0], mat[1]);
    print_matrix(mat[2]);
    
    load_data(mat[3], data[0], 56, 3, 2);
    load_data(mat[4], data[1], 18, 3, 2);
    print_matrix(mat[3]);
    print_matrix(mat[4]);

    minus_matrix(mat[5], mat[0], mat[1]);
    print_matrix(mat[5]);
    
    for (size_t i = 0; i < 2; i++)
        load_data(mat[i], data[i], 0, data[i]->row, data[i]->col);
    print_matrix(mat[0]);
    print_matrix(mat[1]);
}

void t7()
{
    Data *data[2] = {
        data[0] = new Data(8, 6),
        data[1] = new Data(12, 12),
    };
    Matrix mat[4];
    for (size_t i = 0; i < 48; i++)
        data[0]->entry[i] = i * i / 7;
    for (size_t i = 0; i < 144; i++)
        data[1]->entry[i] = i * 7 / (i - i / 2 + 1);
    load_data(mat[0], data[0], 0, 8, 6);
    load_data(mat[1], data[1], 14, 6, 8);
    multiply_matrix(mat[2], mat[0], mat[1]);
    print_matrix(mat[2]);
    multiply_matrix(mat[2], mat[1], mat[0]);
    print_matrix(mat[2]);
    shallow_copy(mat[3], mat[1]);
    
    load_data(mat[1], data[1], 14, 6, 8);
    multiply_matrix(mat[2], mat[0], mat[1]);
    print_matrix(mat[2]);

    print_matrix(mat[0]);
    print_matrix(mat[1]);
}

void t8()
{
    Data *data[3] = {
        new Data(12, 13),
        new Data(2, 7),
        new Data(8, 8),
    };
    Matrix mat[5];
    for (size_t i = 0; i < 156; i++)
        data[0]->entry[i] = i;
    for (size_t i = 0; i < 14; i++)
        data[1]->entry[i] = -i;
    for (size_t i = 0; i < 64; i++)
        data[2]->entry[i] = i * i / (i + 8);
    load_data(mat[0], data[0], 37, 3, 2);
    load_data(mat[1], data[1], 0, 2, 7);
    multiply_matrix(mat[2], mat[0], mat[1]);
    print_matrix(mat[2]);
    
    load_data(mat[3], data[2], 0, 3, 7);
    minus_matrix(mat[4], mat[3], mat[2]);
    print_matrix(mat[4]);
}

void t9()
{
    Data *data = new Data(4, 9);
    Matrix mat, shallow, deep;
    for (size_t i = 0; i < 36; i++)
        data->entry[i] = i;
    load_data(mat, data, 9, 3, 9);
    print_matrix(mat);
    shallow_copy(shallow, mat);
    deep_copy(deep, mat);
    print_matrix(shallow);
    print_matrix(deep);
    cout << shallow.row << ' ' << shallow.row << ": " << shallow.data->row << ' ' << shallow.data->col << '\n' <<
        deep.row << ' ' << deep.row << ": " << deep.data->row << ' ' << deep.data->col << endl;
}

void t10()
{
    Data *data[2] = {
        new Data(4, 3),
        new Data(3, 2),
    };

    for (size_t i = 0; i < 12; i++)
        data[0]->entry[i]  = i / 3 + 2;
    for (size_t i = 0; i < 6; i++)
        data[1]->entry[i]  = i * i;

    Matrix mat[6];
    load_data(mat[0], data[0], 5, 1, 2);
    load_data(mat[1], data[1], 1, 2, 1);
    
    multiply_matrix(mat[2], mat[0], mat[1]);
    print_matrix(mat[2]);

    load_data(mat[3], data[0], 1, 1, 1);
    add_matrix(mat[4], mat[3], mat[2]);
    print_matrix(mat[4]);

    minus_matrix(mat[1], mat[2], mat[3]);
    print_matrix(mat[1]);

    add_matrix(mat[1], mat[2], mat[3]);
    print_matrix(mat[1]);

    deep_copy(mat[4], mat[0]);
    unload_data(mat[4]);
    shallow_copy(mat[4], mat[0]);
    unload_data(mat[0]);
    unload_data(mat[0]);
    unload_data(mat[0]);

    cout << Data::check_cnt() << endl;
}

void (*test[10])() = {t1, t2, t3, t4, t5, t6, t7, t8, t9, t10};

void generate()
{
    for (size_t i = 1; i <= 10; i++)
    {
        streambuf *ccout;
        ofstream in("testcase/" + to_string(i) + ".in", ios::out | ios::trunc),
                 out("testcase/" + to_string(i) + ".out", ios::out | ios::trunc);
        ccout = cout.rdbuf(in.rdbuf());
        cout << i;
        cout.rdbuf(out.rdbuf());
        test[i - 1]();
        cout.rdbuf(ccout);
    }
}

int main()
{
    generate();
    // int id;
    // cin >> id;
    // test[id - 1]();
    return 0;
}
//APPEND END