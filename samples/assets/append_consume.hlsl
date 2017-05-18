AppendStructuredBuffer<uint>  BufferIn : register(u0);
AppendStructuredBuffer<uint>  BufferOut : register(u2);

[numthreads(16, 1, 1)]
void main(uint3 tid : SV_DispatchThreadID)
{
  //uint val = BufferIn.Consume();
  //val |= 0xFF00007F;
  //BufferOut.Append(val);
  uint val = 0xFF0000FF;
  BufferIn.Append(val);

  val = 0xFF00FF00;
  BufferOut.Append(val);
}

/*
struct Data {
  uint  rgba;
};

ConsumeStructuredBuffer<Data>  BufferIn : register(u0);
AppendStructuredBuffer<Data>  BufferOut : register(u2);

[numthreads(16, 1, 1)]
void main(uint3 tid : SV_DispatchThreadID)
{
  Data val = BufferIn.Consume();
  val.rgba = 0xFF000000 | 
             ((val.rgba & 0x000000FF) <<  8) | 
             ((val.rgba & 0x0000FF00) <<  8) | 
             ((val.rgba & 0x00FF0000) >> 16);
  BufferOut.Append(val);
}
*/