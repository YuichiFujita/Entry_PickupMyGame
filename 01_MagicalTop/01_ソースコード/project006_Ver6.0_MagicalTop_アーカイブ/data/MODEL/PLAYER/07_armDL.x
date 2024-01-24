xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 70;
 15.86214;1.97190;-2.12802;,
 15.04752;-0.16544;-0.00000;,
 15.78194;-0.11336;-3.00946;,
 15.89538;2.83565;0.00000;,
 15.04752;-0.16544;-0.00000;,
 15.86214;1.97190;2.12802;,
 15.04752;-0.16544;-0.00000;,
 15.78194;-0.11336;3.00946;,
 15.04752;-0.16544;-0.00000;,
 15.70181;-2.19861;2.12802;,
 15.04752;-0.16544;-0.00000;,
 15.66861;-3.06238;-0.00000;,
 15.04752;-0.16544;-0.00000;,
 15.70181;-2.19861;-2.12802;,
 15.04752;-0.16544;-0.00000;,
 15.78194;-0.11336;-3.00946;,
 15.04752;-0.16544;-0.00000;,
 7.05214;0.06224;-3.29214;,
 6.97275;-2.21888;-2.32789;,
 1.50565;-2.10680;-2.33954;,
 1.47074;0.04918;-3.30859;,
 6.93987;-3.16376;-0.00000;,
 1.47677;-3.05638;-0.00000;,
 7.01883;2.34558;-2.32789;,
 7.05214;0.06224;-3.29214;,
 1.47074;0.04918;-3.30859;,
 1.46842;2.29251;-2.33954;,
 6.97275;-2.21888;2.32789;,
 1.50147;-2.24422;2.33954;,
 6.89123;3.29362;0.00000;,
 1.45880;3.24238;0.00000;,
 7.05214;0.06224;3.29214;,
 1.47074;0.04918;3.30859;,
 7.01883;2.34558;2.32789;,
 1.46842;2.29251;2.33954;,
 11.30256;-0.02611;-3.22710;,
 11.21659;-2.26218;-2.28190;,
 11.18101;-3.18838;-0.00000;,
 11.38853;2.20997;-2.28190;,
 11.30256;-0.02611;-3.22710;,
 11.21659;-2.26218;2.28190;,
 11.42414;3.13618;0.00000;,
 11.30256;-0.02611;3.22710;,
 11.38853;2.20997;2.28190;,
 2.14592;0.01446;-1.64925;,
 2.56533;-0.00019;-0.00000;,
 2.18067;1.15723;-1.16619;,
 2.56533;-0.00019;-0.00000;,
 2.19509;1.63058;0.00000;,
 2.56533;-0.00019;-0.00000;,
 2.18067;1.15723;1.16619;,
 2.56533;-0.00019;-0.00000;,
 2.14592;0.01446;1.64925;,
 2.56533;-0.00019;-0.00000;,
 2.11117;-1.12832;1.16619;,
 2.56533;-0.00019;-0.00000;,
 2.09677;-1.60165;-0.00000;,
 2.56533;-0.00019;-0.00000;,
 2.11117;-1.12832;-1.16619;,
 2.56533;-0.00019;-0.00000;,
 2.14592;0.01446;-1.64925;,
 1.47074;0.04918;-3.30859;,
 1.46842;2.29251;-2.33954;,
 1.45880;3.24238;0.00000;,
 1.46842;2.29251;2.33954;,
 1.47074;0.04918;3.30859;,
 1.50147;-2.24422;2.33954;,
 1.47677;-3.05638;-0.00000;,
 1.50565;-2.10680;-2.33954;,
 1.47074;0.04918;-3.30859;;
 
 48;
 3;0,1,2;,
 3;3,4,0;,
 3;5,6,3;,
 3;7,8,5;,
 3;9,10,7;,
 3;11,12,9;,
 3;13,14,11;,
 3;15,16,13;,
 4;17,18,19,20;,
 4;19,18,21,22;,
 4;23,24,25,26;,
 4;22,21,27,28;,
 4;29,23,26,30;,
 4;28,27,31,32;,
 4;33,29,30,34;,
 4;32,31,33,34;,
 4;35,36,18,17;,
 4;13,36,35,15;,
 4;37,36,13,11;,
 4;18,36,37,21;,
 4;38,39,24,23;,
 4;2,39,38,0;,
 4;40,37,11,9;,
 4;21,37,40,27;,
 4;41,38,23,29;,
 4;0,38,41,3;,
 4;42,40,9,7;,
 4;27,40,42,31;,
 4;43,41,29,33;,
 4;3,41,43,5;,
 4;43,42,7,5;,
 4;31,42,43,33;,
 3;44,45,46;,
 3;46,47,48;,
 3;48,49,50;,
 3;50,51,52;,
 3;52,53,54;,
 3;54,55,56;,
 3;56,57,58;,
 3;58,59,60;,
 4;61,44,46,62;,
 4;62,46,48,63;,
 4;63,48,50,64;,
 4;64,50,52,65;,
 4;65,52,54,66;,
 4;66,54,56,67;,
 4;67,56,58,68;,
 4;68,58,60,69;;
 
 MeshMaterialList {
  1;
  48;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  58;
  0.970995;-0.036277;0.236330;,
  0.965046;-0.203977;0.164558;,
  0.962333;-0.271875;-0.000000;,
  0.965046;-0.203977;-0.164558;,
  0.970995;-0.036277;-0.236330;,
  0.976004;0.136580;-0.169593;,
  0.977781;0.209631;0.000000;,
  0.976004;0.136580;0.169593;,
  0.999397;-0.034732;-0.000000;,
  -0.005070;-0.714077;-0.700049;,
  0.007280;-0.002022;-0.999971;,
  -0.009606;-0.999933;-0.006472;,
  0.012441;0.712520;-0.701541;,
  -0.000657;-0.716655;0.697428;,
  0.012972;0.999916;0.000000;,
  0.008596;0.000764;0.999963;,
  0.012441;0.712520;0.701541;,
  0.017245;-0.712400;-0.701562;,
  0.031846;-0.001041;-0.999492;,
  0.011136;-0.999938;-0.000000;,
  0.045681;0.710804;-0.701905;,
  0.017245;-0.712400;0.701562;,
  0.051034;0.998697;0.000000;,
  0.031846;-0.001041;0.999492;,
  0.045681;0.710804;0.701905;,
  -0.999533;0.030566;-0.000000;,
  -0.949860;0.017232;0.312200;,
  -0.949168;-0.216049;0.228917;,
  -0.948306;-0.317359;0.000000;,
  -0.949168;-0.216049;-0.228917;,
  -0.950326;0.015757;-0.310857;,
  -0.947819;0.239963;-0.209898;,
  -0.942172;0.335113;0.003227;,
  -0.946027;0.243030;0.214403;,
  -0.000647;-0.003695;-0.999993;,
  -0.004544;0.713160;-0.700986;,
  -0.009068;0.999959;0.000000;,
  -0.004544;0.713160;0.700986;,
  0.002005;0.001884;0.999996;,
  -0.001656;-0.721096;0.692833;,
  -0.013388;-0.999827;-0.012911;,
  -0.010465;-0.715916;-0.698108;,
  0.034145;-0.712388;-0.700955;,
  0.048497;-0.001727;-0.998822;,
  0.028070;-0.999606;-0.000000;,
  0.061909;0.709747;-0.701731;,
  0.034145;-0.712388;0.700955;,
  0.067062;0.997749;0.000000;,
  0.048497;-0.001727;0.998822;,
  0.061909;0.709747;0.701731;,
  -0.926297;0.004787;0.376765;,
  -0.916692;-0.282886;0.282227;,
  -0.911632;-0.411007;0.000000;,
  -0.916692;-0.282886;-0.282227;,
  -0.927387;0.001842;-0.374098;,
  -0.930471;0.272305;-0.245099;,
  -0.921397;0.388568;0.006458;,
  -0.926308;0.278290;0.253984;;
  48;
  3;1,8,0;,
  3;2,8,1;,
  3;3,8,2;,
  3;4,8,3;,
  3;5,8,4;,
  3;6,8,5;,
  3;7,8,6;,
  3;0,8,7;,
  4;10,9,41,34;,
  4;41,9,11,40;,
  4;12,10,34,35;,
  4;40,11,13,39;,
  4;14,12,35,36;,
  4;39,13,15,38;,
  4;16,14,36,37;,
  4;38,15,16,37;,
  4;18,17,9,10;,
  4;42,17,18,43;,
  4;19,17,42,44;,
  4;9,17,19,11;,
  4;20,18,10,12;,
  4;43,18,20,45;,
  4;21,19,44,46;,
  4;11,19,21,13;,
  4;22,20,12,14;,
  4;45,20,22,47;,
  4;23,21,46,48;,
  4;13,21,23,15;,
  4;24,22,14,16;,
  4;47,22,24,49;,
  4;24,23,48,49;,
  4;15,23,24,16;,
  3;26,25,27;,
  3;27,25,28;,
  3;28,25,29;,
  3;29,25,30;,
  3;30,25,31;,
  3;31,25,32;,
  3;32,25,33;,
  3;33,25,26;,
  4;50,26,27,51;,
  4;51,27,28,52;,
  4;52,28,29,53;,
  4;53,29,30,54;,
  4;54,30,31,55;,
  4;55,31,32,56;,
  4;56,32,33,57;,
  4;57,33,26,50;;
 }
 MeshTextureCoords {
  70;
  0.125000;1.000000;,
  0.062500;1.000000;,
  0.000000;1.000000;,
  0.250000;1.000000;,
  0.187500;1.000000;,
  0.375000;1.000000;,
  0.312500;1.000000;,
  0.500000;1.000000;,
  0.437500;1.000000;,
  0.625000;1.000000;,
  0.562500;1.000000;,
  0.750000;1.000000;,
  0.687500;1.000000;,
  0.875000;1.000000;,
  0.812500;1.000000;,
  1.000000;1.000000;,
  0.937500;1.000000;,
  1.000000;0.384250;,
  0.875000;0.384250;,
  0.875000;0.000000;,
  1.000000;0.000000;,
  0.750000;0.384250;,
  0.750000;0.000000;,
  0.125000;0.384250;,
  0.000000;0.384250;,
  0.000000;0.000000;,
  0.125000;0.000000;,
  0.625000;0.384250;,
  0.625000;0.000000;,
  0.250000;0.384250;,
  0.250000;0.000000;,
  0.500000;0.384250;,
  0.500000;0.000000;,
  0.375000;0.384250;,
  0.375000;0.000000;,
  1.000000;0.672230;,
  0.875000;0.672230;,
  0.750000;0.672230;,
  0.125000;0.672230;,
  0.000000;0.672230;,
  0.625000;0.672230;,
  0.250000;0.672230;,
  0.500000;0.672230;,
  0.375000;0.672230;,
  0.000000;0.125000;,
  0.062500;0.000000;,
  0.125000;0.125000;,
  0.187500;0.000000;,
  0.250000;0.125000;,
  0.312500;0.000000;,
  0.375000;0.125000;,
  0.437500;0.000000;,
  0.500000;0.125000;,
  0.562500;0.000000;,
  0.625000;0.125000;,
  0.687500;0.000000;,
  0.750000;0.125000;,
  0.812500;0.000000;,
  0.875000;0.125000;,
  0.937500;0.000000;,
  1.000000;0.125000;,
  0.000000;0.250000;,
  0.125000;0.250000;,
  0.250000;0.250000;,
  0.375000;0.250000;,
  0.500000;0.250000;,
  0.625000;0.250000;,
  0.750000;0.250000;,
  0.875000;0.250000;,
  1.000000;0.250000;;
 }
}
