//PREPEND BEGIN
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

struct Data
{
    int *entry;
    size_t row, col;
    size_t ref_cnt;

    Data(size_t row, size_t col):
        row(row), col(col), ref_cnt(0)
    { entry = new int[row * col]{}; }

    ~Data()
    { delete[] entry; }
};

struct Matrix
{
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
    // TODO
    // Noted that `mat.data` could be `nullptr` here
}

void load_data(Matrix &mat, Data *data, size_t start, size_t row, size_t col)
{
    // TODO
}

void shallow_copy(Matrix &dest, Matrix &src)
{
    // TODO
}

void deep_copy(Matrix &dest, Matrix &src)
{
    // TODO
}

bool equal_matrix(Matrix &a, Matrix &b)
{
    // TODO
}

void add_matrix(Matrix &dest, Matrix &a, Matrix &b)
{
    // TODO
}

void minus_matrix(Matrix &dest, Matrix &a, Matrix &b)
{
    // TODO
}

void multiply_matrix(Matrix &dest, Matrix &a, Matrix &b)
{
    // TODO
}
//TEMPLATE END

//APPEND BEGIN
void test()
{
    // Sample code on how to use your library
    Data *da = new Data(3, 2), *db = new Data(2, 3);
    for (size_t i = 0; i < 6; i++)
        da->entry[i] = db->entry[i] = i;

    Matrix a, b, c;
    load_data(a, da, 0, 3, 2);  // the ROI is the whole matrix
    load_data(b, db, 0, 2, 3);
    print_matrix(a);
    /*
        0 1 
        2 3 
        4 5 
    */
    print_matrix(b);
    /*
        0 1 2 
        3 4 5
    */

    multiply_matrix(c, a, b);
    print_matrix(c);
    /*
        3 4 5 
        9 14 19 
        15 24 33
    */

    Matrix d, e, f;
    shallow_copy(d, c); // d, c -> (the same) data
    deep_copy(e, c);    // e->data have the exactly same content with c->ROI(data) with e.data.shape = c.ROI.shape
                        // but their addresses are different and ref_cnts possibly differ
    load_data(f, c.data, 1, 3, 2);
    print_matrix(f);
    /*
        4 5 
        14 19 
        24 33
    */
    add_matrix(b, a, f);   // notice that the original b.data->ref_cnt becomes 0 and should be deleted
    print_matrix(b);
    /*
        4 6 
        16 22 
        28 38
    */

    cout << a.data->ref_cnt << ' ' << b.data->ref_cnt << ' '
        << c.data->ref_cnt << ' ' << d.data->ref_cnt << ' '
        << e.data->ref_cnt << ' ' << f.data->ref_cnt << endl;
    /*
        1 1 3 3 1 3
    */
}

int main()
{
    test();
    return 0;
}
//APPEND END