#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float     width;
uniform float     height;
uniform float     box = 7.0;

vec4 CrossStitching(sampler2D tex, vec2 uv)
{
    vec4 c     = vec4(0.0);
    vec2 cPos  = uv * vec2(width, height);
    vec2 tlPos = floor(cPos / vec2(box, box));
    tlPos *= box;
    int remX = int(mod(cPos.x, box));
    int remY = int(mod(cPos.y, box));
    
    if (remX == 0 && remY == 0)
    {
       tlPos = cPos;
    }
    vec2 blPos = tlPos;
    blPos.y += (box - 1.0);
    
    if (remX == remY || 
        (((int(cPos.x) - int(blPos.x)) == (int(blPos.y) - int(cPos.y)))))
    {
        c = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else 
    {
        c = texture2D(tex, tlPos * vec2(1.0/width, 1.0/height))*1.4;
    }
    return c;
}

vec4 Blur(sampler2D tex, vec2 uv)
{
    const int kernelSize = 5;
    vec2 offset = vec2(0.02, 0.02);
    vec4 outputColor = vec4(0.0);
    for (int i=-kernelSize; i <= kernelSize; ++i)
    {
        for (int j=-kernelSize; j <= kernelSize; ++j)
        {
            float kernelValue = 0.01;
            vec2 sampleLocation = uv + vec2(float(i)*offset.x,float(j)*offset.y);
            vec4 sample = texture2D(tex, sampleLocation);
            outputColor += kernelValue* sample *1.3;
        }
    }
    return outputColor;
}


void main()
{
    FragColor = CrossStitching(texture1, TexCoord);
    //FragColor = Blur(texture1, TexCoord);
} 