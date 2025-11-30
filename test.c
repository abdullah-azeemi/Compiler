// test.c -- simple 2-level nested loop example
void work(int *A, int N, int M) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            // row-major indexing: A[i*M + j]
            A[i * M + j] = i + j;
        }
    }
}
