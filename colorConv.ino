void hsl2rgb(double hsl[], double rgb[]) {
    double min, sv, fract, vsf;

    double v = (hsl[2] <= 0.5) ? (hsl[2] * (1 + hsl[1])) : (hsl[2] + hsl[1] - hsl[2] * hsl[1]);
    if (v == 0.0) {
      rgb[0]=0;
      rgb[1]=0;
      rgb[2]=0;
      return;
    }

    min = 2 * hsl[2] - v;
    sv = (v - min) / v;
    double h = fmod((6 * hsl[0]), 6);
    int sextant = (int)(h);
    fract = h - sextant;
    vsf = v * sv * fract;

    switch (sextant) {
        case 0: rgb[0]=v; rgb[1]=min + vsf; rgb[2]=min; break; // return { v, min + vsf, min };
        case 1: rgb[0]=v - vsf; rgb[1]=v; rgb[2]=min; break;   // return { v - vsf, v, min };
        case 2: rgb[0]=min; rgb[1]=v; rgb[2]=min + vsf; break; // return { min, v, min + vsf };
        case 3: rgb[0]=min; rgb[1]=v - vsf; rgb[2]=v; break;   // return { min, v - vsf, v };
        case 4: rgb[0]=min + vsf; rgb[1]=min; rgb[2]=v; break; // return { min + vsf, min, v };
        case 5: rgb[0]=v; rgb[1]=min; rgb[2]=v - vsf; break;   // return { v, min, v - vsf };
    }
}

void rgb2hsl(double rgb[], double hsl[]){
    double _max = max(rgb[0], max(rgb[1], rgb[2]));
    double _min = min(rgb[0], min(rgb[1], rgb[2]));
    hsl[2] = (_max + _min) / 2;
    if(_max == _min){
        hsl[0] = hsl[1] = 0; // achromatic
    } else {
        double d = _max - _min;
        hsl[1] = hsl[2] > 0.5 ? d / (2 - _max - _min) : d / (_max + _min);
        if (_max == rgb[0]){
          hsl[0] = (rgb[1] - rgb[2]) / d + (rgb[1] < rgb[2] ? 6 : 0);
        } else if (_max == rgb[1]) {
          hsl[0] = (rgb[2] - rgb[0]) / d + 2;
        } else if (_max == rgb[2]) {
          hsl[0] = (rgb[0] - rgb[1]) / d + 4;
        }
        hsl[0] /= 6.0;
    }
}

