varying vec2 TexCoord;

varying vec4 clipSpace;

uniform sampler2D backgroundTex;
uniform sampler2D maskTex;

void main(void) {
  vec2 query = (clipSpace.xy * 0.5) + vec2(0.5, 0.5);
  query.y = -query.y;

  vec4 mask = texture2D(maskTex, query);
  vec4 sample = texture2D(backgroundTex, TexCoord.xy);
  // sample.r = 0.5;

  gl_FragColor.rgb = sample.rgb;
  gl_FragColor.a = sample.a * mask.r;
  // gl_FragColor = mask;
  // gl_FragColor.a = 1.0;
  gl_FragColor.a *= 0.6;
  // gl_FragColor.r = 0.5;
}