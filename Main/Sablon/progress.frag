    #version 330 core
    out vec4 FragColor;
    uniform float barColor;
    void main() {
        if (gl_FragCoord.y >= 0.0  && gl_FragCoord.x >= barColor && gl_FragCoord.x <= 1050.0 && gl_FragCoord.y <= 900)
 {
            FragColor = vec4(1.0, 1.0, 0.0, 1.0);
        } else {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    }
 