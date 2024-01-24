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
 60;
 0.25846;-2.53179;2.32477;,
 0.25846;-2.53179;0.00000;,
 1.89900;-2.39692;1.64386;,
 -1.38207;-2.66665;1.64386;,
 -2.06161;-2.72251;0.00000;,
 -1.38207;-2.66665;-1.64386;,
 0.25846;-2.53179;-2.32477;,
 1.89900;-2.39692;-1.64386;,
 2.57854;-2.34107;-0.00000;,
 -0.02803;1.26695;2.48362;,
 1.72461;1.41103;1.75618;,
 -1.78066;1.12288;1.75618;,
 -2.50663;1.06319;0.00000;,
 -1.78066;1.12288;-1.75618;,
 -0.02803;1.26695;-2.48362;,
 1.72461;1.41103;-1.75618;,
 2.45058;1.47071;0.00000;,
 -1.43326;1.99318;-1.35357;,
 -1.63719;1.63117;-1.58664;,
 -2.29307;1.57725;0.00000;,
 -1.99280;1.94718;0.00000;,
 -0.08243;2.10423;-1.91423;,
 -0.05376;1.76133;-2.24384;,
 2.18555;1.94541;0.00000;,
 1.82793;2.26128;0.00000;,
 1.26840;2.21529;1.35357;,
 1.52968;1.89150;1.58664;,
 -1.63719;1.63117;1.58664;,
 -1.43326;1.99318;1.35357;,
 1.26840;2.21529;-1.35357;,
 1.52968;1.89150;-1.58664;,
 -0.08243;2.10423;1.91423;,
 -0.05376;1.76133;2.24384;,
 -0.09769;2.25652;0.00000;,
 0.59048;-6.46493;2.11736;,
 2.08201;-6.34192;1.49720;,
 2.13204;-5.57171;1.61237;,
 0.52576;-5.70418;2.28024;,
 -0.90106;-6.58793;1.49720;,
 -1.08052;-5.83665;1.61237;,
 -1.51888;-6.63889;0.00000;,
 -1.74587;-5.89151;0.00000;,
 -0.90106;-6.58793;-2.58710;,
 -1.08052;-5.83665;-1.61237;,
 0.59048;-6.46493;-3.20726;,
 0.52576;-5.70418;-2.28024;,
 2.08201;-6.34192;-2.58710;,
 2.13204;-5.57171;-1.61237;,
 2.69983;-6.29098;-0.00000;,
 2.79738;-5.51683;-0.00000;,
 0.59048;-6.46493;-0.00000;,
 1.96301;-2.22669;1.72755;,
 0.24198;-2.36861;-0.00000;,
 0.24198;-2.36861;2.44313;,
 -1.47904;-2.51053;1.72755;,
 -2.19191;-2.56933;0.00000;,
 -1.47904;-2.51053;-1.72755;,
 0.24198;-2.36861;-2.44313;,
 1.96301;-2.22669;-1.72755;,
 2.67588;-2.16788;0.00000;;
 
 72;
 3;0,1,2;,
 3;3,1,0;,
 3;4,1,3;,
 3;5,1,4;,
 3;6,1,5;,
 3;7,1,6;,
 3;8,1,7;,
 3;2,1,8;,
 4;9,0,2,10;,
 4;11,3,0,9;,
 4;12,4,3,11;,
 4;13,5,4,12;,
 4;14,6,5,13;,
 4;15,7,6,14;,
 4;16,8,7,15;,
 4;10,2,8,16;,
 4;17,18,19,20;,
 4;18,17,21,22;,
 4;23,24,25,26;,
 4;20,19,27,28;,
 4;22,21,29,30;,
 4;26,25,31,32;,
 4;30,29,24,23;,
 4;32,31,28,27;,
 4;14,13,18,22;,
 4;15,14,22,30;,
 4;16,15,30,23;,
 4;10,16,23,26;,
 4;9,10,26,32;,
 4;9,32,27,11;,
 4;19,12,11,27;,
 4;18,13,12,19;,
 3;24,33,25;,
 3;29,33,24;,
 3;21,33,29;,
 3;17,33,21;,
 3;33,17,20;,
 3;33,20,28;,
 3;31,33,28;,
 3;25,33,31;,
 4;34,35,36,37;,
 4;38,34,37,39;,
 4;40,38,39,41;,
 4;42,40,41,43;,
 4;44,42,43,45;,
 4;46,44,45,47;,
 4;48,46,47,49;,
 4;35,48,49,36;,
 3;34,50,35;,
 3;38,50,34;,
 3;40,50,38;,
 3;42,50,40;,
 3;44,50,42;,
 3;46,50,44;,
 3;48,50,46;,
 3;35,50,48;,
 3;51,52,53;,
 3;53,52,54;,
 3;54,52,55;,
 3;55,52,56;,
 3;56,52,57;,
 3;57,52,58;,
 3;58,52,59;,
 3;59,52,51;,
 4;49,59,51,36;,
 4;47,58,59,49;,
 4;45,57,58,47;,
 4;43,56,57,45;,
 4;41,55,56,43;,
 4;39,54,55,41;,
 4;37,53,54,39;,
 4;36,51,53,37;;
 
 MeshMaterialList {
  1;
  72;
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
   TextureFilename {
    "02_data\\03_MODEL\\00_TEXTURE\\Chara_UV.png";
   }
  }
 }
 MeshNormals {
  85;
  0.081931;-0.996638;0.000003;,
  0.081932;-0.996638;-0.000000;,
  0.081928;-0.996638;-0.000000;,
  0.081928;-0.996638;-0.000000;,
  0.081928;-0.996638;0.000000;,
  0.081931;-0.996638;-0.000003;,
  0.081928;-0.996638;-0.000000;,
  0.081930;-0.996638;-0.000000;,
  0.675458;0.250365;0.693595;,
  -0.015996;0.204414;0.978754;,
  -0.705037;0.158774;0.691168;,
  -0.990161;0.139932;0.000000;,
  -0.705037;0.158774;-0.691168;,
  -0.015996;0.204414;-0.978754;,
  0.675458;0.250365;-0.693595;,
  0.962995;0.269521;0.000000;,
  -0.082000;0.996632;0.000000;,
  -0.621264;0.530036;-0.577142;,
  -0.046132;0.570422;-0.820055;,
  0.534793;0.611657;-0.582986;,
  0.777399;0.629007;0.000000;,
  0.534793;0.611657;0.582986;,
  -0.046132;0.570422;0.820055;,
  -0.621264;0.530036;0.577142;,
  -0.858071;0.513531;0.000000;,
  0.222359;0.928286;0.298062;,
  0.345221;0.938522;0.000000;,
  0.222359;0.928286;-0.298062;,
  -0.074344;0.903687;-0.421691;,
  -0.371366;0.879239;-0.298372;,
  -0.494548;0.869151;0.000000;,
  -0.371366;0.879239;0.298372;,
  -0.074344;0.903687;0.421691;,
  0.706608;-0.149322;0.691671;,
  0.017225;-0.207928;0.977992;,
  -0.671727;-0.266422;0.691232;,
  -0.979355;-0.198965;0.035721;,
  -0.945695;-0.102938;-0.308326;,
  -0.063571;0.769282;-0.635739;,
  0.192385;0.765129;-0.614463;,
  0.998880;-0.030858;0.035855;,
  -0.082188;0.996617;0.000005;,
  -0.082184;0.996617;0.000000;,
  -0.082187;0.996617;-0.000003;,
  -0.082190;0.996617;0.000000;,
  -0.082187;0.996617;0.000003;,
  -0.082188;0.996617;-0.000005;,
  -0.082194;0.996616;0.000000;,
  0.082189;-0.996617;0.000001;,
  -0.082188;0.996617;0.000000;,
  0.709566;-0.069280;0.701225;,
  0.999834;0.002494;0.018051;,
  0.319486;0.416717;-0.851044;,
  -0.034179;0.413666;-0.909787;,
  -0.806466;-0.109169;-0.581115;,
  -0.985904;-0.166340;0.017984;,
  -0.687965;-0.188092;0.700947;,
  0.010663;-0.128722;0.991623;,
  0.003371;-0.041526;0.999132;,
  0.707415;0.011552;0.706703;,
  -0.700906;-0.094676;0.706942;,
  -0.993161;-0.116749;0.000000;,
  -0.700906;-0.094676;-0.706942;,
  0.003371;-0.041526;-0.999132;,
  0.707415;0.011552;-0.706703;,
  0.999437;0.033551;0.000000;,
  -0.315333;0.722674;-0.615066;,
  0.949473;0.058137;-0.308418;,
  0.937259;0.045859;-0.345604;,
  0.082190;-0.996617;-0.000000;,
  0.082190;-0.996617;0.000003;,
  0.082189;-0.996617;0.000003;,
  0.082192;-0.996617;0.000001;,
  0.082189;-0.996617;-0.000002;,
  0.082190;-0.996617;-0.000000;,
  0.082190;-0.996617;-0.000002;,
  0.082187;-0.996617;0.000001;,
  0.999343;0.036256;0.000000;,
  0.707895;0.011454;0.706224;,
  0.707895;0.011454;-0.706224;,
  0.004012;-0.048435;-0.998818;,
  -0.699764;-0.108294;-0.706118;,
  -0.991107;-0.133068;0.000000;,
  -0.699764;-0.108294;0.706118;,
  0.004012;-0.048435;0.998818;;
  72;
  3;1,7,0;,
  3;2,7,1;,
  3;3,7,2;,
  3;4,7,3;,
  3;1,7,4;,
  3;5,7,1;,
  3;6,7,5;,
  3;0,7,6;,
  4;9,58,59,8;,
  4;10,60,58,9;,
  4;11,61,60,10;,
  4;12,62,61,11;,
  4;13,63,62,12;,
  4;14,64,63,13;,
  4;15,65,64,14;,
  4;8,59,65,15;,
  4;29,17,24,30;,
  4;17,29,28,18;,
  4;20,26,25,21;,
  4;30,24,23,31;,
  4;18,28,27,19;,
  4;21,25,32,22;,
  4;19,27,26,20;,
  4;22,32,31,23;,
  4;13,12,17,18;,
  4;14,13,18,19;,
  4;15,14,19,20;,
  4;8,15,20,21;,
  4;9,8,21,22;,
  4;9,22,23,10;,
  4;24,11,10,23;,
  4;17,12,11,24;,
  3;26,16,25;,
  3;27,16,26;,
  3;28,16,27;,
  3;29,16,28;,
  3;16,29,30;,
  3;16,30,31;,
  3;32,16,31;,
  3;25,16,32;,
  4;34,33,50,57;,
  4;35,34,57,56;,
  4;36,35,56,55;,
  4;37,36,55,54;,
  4;38,66,66,53;,
  4;39,38,53,52;,
  4;40,67,68,51;,
  4;33,40,51,50;,
  3;69,48,70;,
  3;71,48,69;,
  3;72,48,71;,
  3;73,48,72;,
  3;74,48,73;,
  3;75,48,74;,
  3;76,48,75;,
  3;70,48,76;,
  3;41,49,42;,
  3;42,49,43;,
  3;43,49,44;,
  3;44,49,45;,
  3;45,49,42;,
  3;42,49,46;,
  3;46,49,47;,
  3;47,49,41;,
  4;51,77,78,50;,
  4;68,79,77,51;,
  4;53,80,79,52;,
  4;54,81,80,53;,
  4;55,82,81,54;,
  4;56,83,82,55;,
  4;57,84,83,56;,
  4;50,78,84,57;;
 }
 MeshTextureCoords {
  60;
  0.867870;0.366150;,
  0.841704;0.366150;,
  0.860206;0.364632;,
  0.860206;0.367668;,
  0.841704;0.368297;,
  0.823202;0.367668;,
  0.815538;0.366150;,
  0.823202;0.364632;,
  0.841704;0.364004;,
  0.869658;0.323394;,
  0.861470;0.321773;,
  0.861470;0.325016;,
  0.841704;0.325687;,
  0.821937;0.325016;,
  0.813750;0.323394;,
  0.821937;0.321773;,
  0.841704;0.321101;,
  0.826469;0.315220;,
  0.823846;0.319295;,
  0.841704;0.319902;,
  0.841704;0.315738;,
  0.820158;0.313971;,
  0.816449;0.317830;,
  0.841704;0.315758;,
  0.841704;0.312203;,
  0.856939;0.312720;,
  0.859562;0.316365;,
  0.859562;0.319295;,
  0.856939;0.315220;,
  0.826469;0.312720;,
  0.823846;0.316365;,
  0.863249;0.313971;,
  0.866959;0.317830;,
  0.841704;0.312256;,
  0.843174;0.049060;,
  0.862572;0.057125;,
  0.863223;0.055627;,
  0.842332;0.046941;,
  0.823775;0.057125;,
  0.821441;0.055627;,
  0.815740;0.076598;,
  0.812787;0.076598;,
  0.823775;0.110245;,
  0.821441;0.097568;,
  0.843174;0.118311;,
  0.842332;0.106254;,
  0.862572;0.110245;,
  0.863223;0.097568;,
  0.870608;0.076598;,
  0.871877;0.076598;,
  0.843174;0.076598;,
  0.861025;0.054129;,
  0.838641;0.076598;,
  0.838641;0.044823;,
  0.816258;0.054129;,
  0.806986;0.076598;,
  0.816258;0.099066;,
  0.838641;0.108373;,
  0.861025;0.099066;,
  0.870296;0.076598;;
 }
}
