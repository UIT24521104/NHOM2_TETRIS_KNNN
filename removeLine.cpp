void removeLine()

{
    for (int i = H - 2; i >= 1; i--)
        {
            bool full = true;
            for (int j = 1; j < W - 1; j++)
                if (board[i][j] == ' ')  
                { 
                    full = false; 
                    break; 
                }
            if (full) 
            {
                for (int k = i; k > 0; k--)
                    for (int j = 1; j < W - 1; j++)
                        board[k][j] = board[k - 1][j];
                i++;
            }
        }
}

