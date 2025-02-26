#define PI 3.14159265358979323846f

int GetNumMips(TextureCube aCube)
{
    int w = 0;
    int h = 0;
    int m = 0;
    aCube.GetDimensions(0, w, h, m);
    return m;
}