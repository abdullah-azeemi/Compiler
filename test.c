void work(int *A, int N, int M) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            A[i*M + j] = i + j;
}