float2 GetTexcoord(float2 texcoord)
{
    texcoord.y = 1 - texcoord.y;
    return texcoord;
}