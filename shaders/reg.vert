varying vec2 TexCoord;

varying vec4 clipSpace;

void main(void) {
  TexCoord = gl_MultiTexCoord0.st;
  gl_Position = ftransform();
  clipSpace = gl_Position;
}