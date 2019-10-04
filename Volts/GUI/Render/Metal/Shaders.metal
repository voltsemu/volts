vertex float4 VertexShader(
    const device packed_float3* VertexArray [[ buffer(0) ]],
    unsigned int ID [[ vertex_id ]]
)
{
    return float4(VertexArray[ID], 1.f);
}

fragment half4 FragmentShader()
{
    return half4(1.f);
}