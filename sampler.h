#pragma once
class Sampler {
private:
    int hai2id[34], max_haiid;
    int hai[34], rest[34];
    int f[10][2][3][3][34];
    int ff[8][2][34];
    void procNum(int offset, int max_round);
    void procZ(int max_round);
    int sample1(const int seires[], const int len);
public:
    Sampler(const int hai[], const int rest[], int seed);
    void sample(double result[], int times = 100000, int rest_round = -1);
};