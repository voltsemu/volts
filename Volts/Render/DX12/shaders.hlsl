struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Colour : COLOR;
};

PSInput VSMain(float4 Pos : POSITION, float4 Colour : COLOR)
{
    PSInput Res;

    Res.Position = Pos;
    Res.Colour = Colour;

    return Res;
}

float4 PSMain(PSInput Input) : SV_TARGET
{
    return Input.Colour;
}