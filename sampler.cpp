#include "sampler.h"
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <algorithm>

Sampler::Sampler(const int hai[], const int rest[], int seed) {
    srand(seed);
    for (int i = 0; i < 34; ++ i) {
        this->hai[i] = hai[i];
        this->rest[i] = rest[i];
        this->hai2id[i] = 0;
    }
    this->max_haiid = 0;
    for (int i = 0; i < 34; ++ i) {
        if (this->hai[i] > 0) {
            this->hai2id[i] = ++ this->max_haiid;
        }
    }
}

void Sampler::procNum(int offset, int max_round) {
    memset(this->f, 0, sizeof(int) * 6120); // 10 * 2 * 3 * 3 * 34
    f[0][0][0][0][0] = 1;
    for (int i = 0; i < 9; ++ i) {
        int orr = 0;
        if (hai2id[i + offset] > 0) {
            orr = 1 << hai2id[i + offset];
        }
        for (int j = 0; j < 3; ++ j) {
            for (int k = 0; k < 3; ++ k) {
                for (int m = 0; m <= max_round; ++ m) {
                    if ((f[i][0][j][k][m] | f[i][1][j][k][m]) & 1) {
                        for (int o = 0; o <= hai[i + offset] - j - k && o + m <= max_round; ++ o) {
                            if (f[i][0][j][k][m] & 1) {
                                 f[i + 1][0][k][ (hai[i + offset] - j - k - o) % 3 ][m + o] |= f[i][0][j][k][m] | (o ? orr : 0);
                                 if (hai[i + offset] - j - k - o >= 2) {
                                    f[i + 1][1][k][ (hai[i + offset] - j - k - o - 2) % 3 ][m + o] |= f[i][0][j][k][m] | (o ? orr : 0);
                                 }
                            }
                            if (f[i][1][j][k][m] & 1) {
                                f[i + 1][1][k][ (hai[i + offset] - j - k - o) % 3 ][m + o] |= f[i][1][j][k][m] | (o ? orr : 0);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Sampler::procZ(int max_round) {
    memset(this->ff, 0, sizeof(int) * 544);
    int offset = 27;
    ff[0][0][0] = 1;
    for (int i = 0; i < 7; ++ i) {
        int orr = 0;
        if (hai2id[i + offset] > 0) {
            orr = 1 << hai2id[i + offset];
        }
        for (int m = 0; m <= max_round; ++ m) {
            if ((ff[i][0][m] | ff[i][1][m]) & 1) {
                for (int o = 0; o <= hai[i + offset] && o + m <= max_round; ++ o) {
                    if (ff[i][0][m] & 1) {
                        if (hai[i + offset] - o == 0 || hai[i + offset] - o == 3) { // 0 or 3
                            ff[i + 1][0][m + o] |= ff[i][0][m] | (o ? orr : 0);
                        }
                        if (hai[i + offset] - o == 2) { // 2
                            ff[i + 1][1][m + o] |= ff[i][0][m] | (o ? orr : 0);
                        }
                    }
                    if (ff[i][1][m] & 1) {
                        if (hai[i + offset] - o == 0 || hai[i + offset] - o == 3) {
                            ff[i + 1][1][m + o] |= ff[i][1][m] | (o ? orr : 0);
                        }
                    }
                }
            }
        }
    }
}

int Sampler::sample1(const int seires[], const int len) {
    int *g[2];
    int *ng[2];
    g[0] = new int[len + 1];
    g[1] = new int[len + 1];

    ng[0] = new int[len + 1];
    ng[1] = new int[len + 1];

    for (int i = 0; i < len; ++ i) {
        this->hai[ seires[i] ] ++;
    }

    this->procNum(0, len);

    for (int i = 0; i <= len; ++ i) {
        g[0][i] = f[9][0][0][0][i];
        g[1][i] = f[9][1][0][0][i];
    }
    this->procNum(9, len);
    memset(ng[0], 0, sizeof(int) * (len + 1));
    memset(ng[1], 0, sizeof(int) * (len + 1));
    for (int i = 0; i <= len; ++ i) {
        for (int j = 0; j + i <= len; ++ j) {
            if (g[0][i] & f[9][0][0][0][j] & 1) {
                ng[0][i + j] |= g[0][i] | f[9][0][0][0][j];
            }
            if (g[0][i] & f[9][1][0][0][j] & 1) {
                ng[1][i + j] |= g[0][i] | f[9][1][0][0][j];
            }
            if (g[1][i] & f[9][0][0][0][j] & 1) {
                ng[1][i + j] |= g[1][i] | f[9][0][0][0][j];
            }
        }
    }
    for (int i = 0; i <= len; ++ i) {
        g[0][i] = ng[0][i];
        g[1][i] = ng[1][i];
    }

    this->procNum(18, len);
    memset(ng[0], 0, sizeof(int) * (len + 1));
    memset(ng[1], 0, sizeof(int) * (len + 1));
    for (int i = 0; i <= len; ++ i) {
        for (int j = 0; j + i <= len; ++ j) {
            if (g[0][i] & f[9][0][0][0][j] & 1) {
                ng[0][i + j] |= g[0][i] | f[9][0][0][0][j];
            }
            if (g[0][i] & f[9][1][0][0][j] & 1) {
                ng[1][i + j] |= g[0][i] | f[9][1][0][0][j];
            }
            if (g[1][i] & f[9][0][0][0][j] & 1) {
                ng[1][i + j] |= g[1][i] | f[9][0][0][0][j];
            }
        }
    }
    for (int i = 0; i <= len; ++ i) {
        g[0][i] = ng[0][i];
        g[1][i] = ng[1][i];
    }

    this->procZ(len);
    memset(ng[0], 0, sizeof(int) * (len + 1));
    memset(ng[1], 0, sizeof(int) * (len + 1));
    for (int i = 0; i <= len; ++ i) {
        for (int j = 0; j + i <= len; ++ j) {
            if (g[0][i] & ff[7][0][j] & 1) {
                ng[0][i + j] |= g[0][i] | ff[7][0][j];
            }
            if (g[0][i] & ff[7][1][j] & 1) {
                ng[1][i + j] |= g[0][i] | ff[7][1][j];
            }
            if (g[1][i] & ff[7][0][j] & 1) {
                ng[1][i + j] |= g[1][i] | ff[7][0][j];
            }
        }
    }

    for (int i = 0; i < len; ++ i) {
        this->hai[ seires[i] ] --;
    }

    int ret = ng[1][len];
    delete [] g[0];
    delete [] g[1];
    delete [] ng[0];
    delete [] ng[1];
    return ret;
}

void Sampler::sample(double result[], int times, int rr) {
    for (int i = 0; i < 34; ++ i) {
        result[i] = 0;
    }

    int hai_list[20];
    for (int i = 0; i < 34; ++ i) hai_list[ this->hai2id[i] ] = i;

    int rest_round = 0, rest_hainum = 0;
    for (int i = 0; i < 34; ++i) rest_hainum += this->rest[i];
    rest_round = (rest_hainum - 14 - 13 * 3) / 4;
    if (rr != -1) rest_round = rr;
    else {
        rest_round = (rest_round > 0) ? rest_round : 0;
    }

    int *series = new int [rest_hainum];
    for (int tmp = 0, i = 0; i < 34; ++ i) {
        for (int j = 0; j < this->rest[i]; ++ j)
            series[tmp ++] = i;
    }
    

    for (int t = 0; t < times; ++ t) {
        /* shuffle */
        std::random_shuffle(series, series + rest_hainum);

        /* calc */
        int res = this->sample1(series, rest_round);

        if (res & 1) {
            for (int i = 1; i <= max_haiid; ++ i) {
                if ((res >> i) & 1) { 
                    result[ hai_list[i] ] += 1;
                }
            }
        }
    }

    for (int i = 0; i < 34; ++ i) {
        result[i] /= times;
    }
    delete[] series;
}