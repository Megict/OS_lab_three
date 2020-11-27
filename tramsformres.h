char* transform(int inp)
{
    int i=0;
    char* str = (char*)malloc(sizeof(char)*1);
    while(inp != 0)
    {
        str = (char*)realloc(str,sizeof(char)*(i+1));
        str[i] = (inp%10)+'0';
        ++i; 
        inp/=10;
    }
    str[i] = 0;
    return str;
}

int back_transform(char* inp)
{
    int res = 0;
    int mult = 1;
    int i = 0;
    while(inp[i]!= 0)
    {
        res += (inp[i]-'0')*mult;
        ++i;
        mult*=10;
    }
    //free(inp);
    return res;
}