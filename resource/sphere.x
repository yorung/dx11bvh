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
 180;
 0.00000;100.00000;0.00000;,
 12.94095;96.59258;-22.41439;,
 0.00000;96.59258;-25.88190;,
 0.00000;100.00000;0.00000;,
 22.41439;96.59258;-12.94095;,
 0.00000;100.00000;0.00000;,
 25.88190;96.59258;0.00000;,
 0.00000;100.00000;0.00000;,
 22.41439;96.59258;12.94095;,
 0.00000;100.00000;0.00000;,
 12.94095;96.59258;22.41439;,
 0.00000;100.00000;0.00000;,
 0.00000;96.59258;25.88190;,
 0.00000;100.00000;0.00000;,
 -12.94095;96.59258;22.41439;,
 0.00000;100.00000;0.00000;,
 -22.41438;96.59258;12.94096;,
 0.00000;100.00000;0.00000;,
 -25.88190;96.59258;0.00001;,
 0.00000;100.00000;0.00000;,
 -22.41439;96.59258;-12.94094;,
 0.00000;100.00000;0.00000;,
 -12.94097;96.59258;-22.41438;,
 0.00000;100.00000;0.00000;,
 0.00000;96.59258;-25.88190;,
 25.00000;86.60254;-43.30127;,
 0.00000;86.60254;-50.00000;,
 43.30127;86.60254;-25.00000;,
 50.00000;86.60254;0.00000;,
 43.30127;86.60254;25.00000;,
 25.00000;86.60254;43.30127;,
 0.00001;86.60254;50.00000;,
 -24.99999;86.60254;43.30128;,
 -43.30126;86.60254;25.00002;,
 -50.00000;86.60254;0.00002;,
 -43.30128;86.60254;-24.99998;,
 -25.00003;86.60254;-43.30125;,
 0.00000;86.60254;-50.00000;,
 35.35534;70.71068;-61.23724;,
 0.00000;70.71068;-70.71068;,
 61.23725;70.71068;-35.35534;,
 70.71068;70.71068;0.00000;,
 61.23724;70.71068;35.35534;,
 35.35534;70.71068;61.23724;,
 0.00001;70.71068;70.71068;,
 -35.35532;70.71068;61.23725;,
 -61.23723;70.71068;35.35536;,
 -70.71068;70.71068;0.00003;,
 -61.23726;70.71068;-35.35530;,
 -35.35538;70.71068;-61.23722;,
 0.00000;70.71068;-70.71068;,
 43.30127;50.00000;-75.00001;,
 0.00000;50.00000;-86.60255;,
 75.00001;50.00000;-43.30127;,
 86.60255;50.00000;0.00000;,
 75.00000;50.00000;43.30128;,
 43.30128;50.00000;75.00000;,
 0.00001;50.00000;86.60255;,
 -43.30125;50.00000;75.00002;,
 -74.99998;50.00000;43.30130;,
 -86.60255;50.00000;0.00004;,
 -75.00003;50.00000;-43.30123;,
 -43.30132;50.00000;-74.99998;,
 0.00000;50.00000;-86.60255;,
 48.29629;25.88191;-83.65163;,
 0.00000;25.88191;-96.59258;,
 83.65163;25.88191;-48.29629;,
 96.59258;25.88191;0.00000;,
 83.65163;25.88191;48.29630;,
 48.29630;25.88191;83.65163;,
 0.00001;25.88191;96.59258;,
 -48.29627;25.88191;83.65164;,
 -83.65161;25.88191;48.29632;,
 -96.59258;25.88191;0.00004;,
 -83.65166;25.88191;-48.29624;,
 -48.29635;25.88191;-83.65160;,
 0.00000;25.88191;-96.59258;,
 50.00000;0.00001;-86.60254;,
 0.00000;0.00001;-100.00000;,
 86.60255;0.00001;-50.00000;,
 100.00000;0.00001;0.00000;,
 86.60254;0.00001;50.00001;,
 50.00001;0.00001;86.60254;,
 0.00002;0.00001;100.00000;,
 -49.99998;0.00001;86.60255;,
 -86.60252;0.00001;50.00003;,
 -100.00000;0.00001;0.00005;,
 -86.60257;0.00001;-49.99995;,
 -50.00006;0.00001;-86.60251;,
 0.00000;0.00001;-100.00000;,
 48.29630;-25.88189;-83.65163;,
 0.00000;-25.88189;-96.59259;,
 83.65164;-25.88189;-48.29629;,
 96.59259;-25.88189;0.00000;,
 83.65163;-25.88189;48.29630;,
 48.29630;-25.88189;83.65163;,
 0.00001;-25.88189;96.59259;,
 -48.29627;-25.88189;83.65164;,
 -83.65161;-25.88189;48.29632;,
 -96.59259;-25.88189;0.00004;,
 -83.65166;-25.88189;-48.29625;,
 -48.29635;-25.88189;-83.65160;,
 0.00000;-25.88189;-96.59259;,
 43.30128;-49.99998;-75.00001;,
 0.00000;-49.99998;-86.60255;,
 75.00002;-49.99998;-43.30127;,
 86.60255;-49.99998;0.00000;,
 75.00001;-49.99998;43.30128;,
 43.30128;-49.99998;75.00001;,
 0.00001;-49.99998;86.60255;,
 -43.30126;-49.99998;75.00002;,
 -74.99999;-49.99998;43.30130;,
 -86.60255;-49.99998;0.00004;,
 -75.00004;-49.99998;-43.30124;,
 -43.30133;-49.99998;-74.99998;,
 0.00000;-49.99998;-86.60255;,
 35.35535;-70.71066;-61.23726;,
 0.00000;-70.71066;-70.71069;,
 61.23726;-70.71066;-35.35534;,
 70.71069;-70.71066;0.00000;,
 61.23726;-70.71066;35.35535;,
 35.35535;-70.71066;61.23726;,
 0.00001;-70.71066;70.71069;,
 -35.35533;-70.71066;61.23727;,
 -61.23725;-70.71066;35.35537;,
 -70.71069;-70.71066;0.00003;,
 -61.23727;-70.71066;-35.35531;,
 -35.35539;-70.71066;-61.23723;,
 0.00000;-70.71066;-70.71069;,
 25.00001;-86.60252;-43.30129;,
 0.00000;-86.60252;-50.00002;,
 43.30129;-86.60252;-25.00001;,
 50.00002;-86.60252;0.00000;,
 43.30129;-86.60252;25.00002;,
 25.00002;-86.60252;43.30129;,
 0.00001;-86.60252;50.00002;,
 -25.00000;-86.60252;43.30130;,
 -43.30128;-86.60252;25.00003;,
 -50.00002;-86.60252;0.00002;,
 -43.30130;-86.60252;-24.99999;,
 -25.00004;-86.60252;-43.30128;,
 0.00000;-86.60252;-50.00002;,
 12.94097;-96.59258;-22.41442;,
 0.00000;-96.59258;-25.88194;,
 22.41442;-96.59258;-12.94097;,
 25.88194;-96.59258;0.00000;,
 22.41442;-96.59258;12.94097;,
 12.94097;-96.59258;22.41442;,
 0.00000;-96.59258;25.88194;,
 -12.94096;-96.59258;22.41442;,
 -22.41441;-96.59258;12.94098;,
 -25.88194;-96.59258;0.00001;,
 -22.41442;-96.59258;-12.94096;,
 -12.94098;-96.59258;-22.41441;,
 0.00000;-96.59258;-25.88194;,
 0.00000;-96.59258;-25.88194;,
 12.94097;-96.59258;-22.41442;,
 0.00000;-100.00000;-0.00000;,
 22.41442;-96.59258;-12.94097;,
 0.00000;-100.00000;-0.00000;,
 25.88194;-96.59258;0.00000;,
 0.00000;-100.00000;-0.00000;,
 22.41442;-96.59258;12.94097;,
 0.00000;-100.00000;-0.00000;,
 12.94097;-96.59258;22.41442;,
 0.00000;-100.00000;-0.00000;,
 0.00000;-96.59258;25.88194;,
 0.00000;-100.00000;-0.00000;,
 -12.94096;-96.59258;22.41442;,
 0.00000;-100.00000;-0.00000;,
 -22.41441;-96.59258;12.94098;,
 0.00000;-100.00000;-0.00000;,
 -25.88194;-96.59258;0.00001;,
 0.00000;-100.00000;-0.00000;,
 -22.41442;-96.59258;-12.94096;,
 0.00000;-100.00000;-0.00000;,
 -12.94098;-96.59258;-22.41441;,
 0.00000;-100.00000;-0.00000;,
 0.00000;-96.59258;-25.88194;,
 0.00000;-100.00000;-0.00000;;
 
 144;
 3;0,1,2;,
 3;3,4,1;,
 3;5,6,4;,
 3;7,8,6;,
 3;9,10,8;,
 3;11,12,10;,
 3;13,14,12;,
 3;15,16,14;,
 3;17,18,16;,
 3;19,20,18;,
 3;21,22,20;,
 3;23,24,22;,
 4;2,1,25,26;,
 4;1,4,27,25;,
 4;4,6,28,27;,
 4;6,8,29,28;,
 4;8,10,30,29;,
 4;10,12,31,30;,
 4;12,14,32,31;,
 4;14,16,33,32;,
 4;16,18,34,33;,
 4;18,20,35,34;,
 4;20,22,36,35;,
 4;22,24,37,36;,
 4;26,25,38,39;,
 4;25,27,40,38;,
 4;27,28,41,40;,
 4;28,29,42,41;,
 4;29,30,43,42;,
 4;30,31,44,43;,
 4;31,32,45,44;,
 4;32,33,46,45;,
 4;33,34,47,46;,
 4;34,35,48,47;,
 4;35,36,49,48;,
 4;36,37,50,49;,
 4;39,38,51,52;,
 4;38,40,53,51;,
 4;40,41,54,53;,
 4;41,42,55,54;,
 4;42,43,56,55;,
 4;43,44,57,56;,
 4;44,45,58,57;,
 4;45,46,59,58;,
 4;46,47,60,59;,
 4;47,48,61,60;,
 4;48,49,62,61;,
 4;49,50,63,62;,
 4;52,51,64,65;,
 4;51,53,66,64;,
 4;53,54,67,66;,
 4;54,55,68,67;,
 4;55,56,69,68;,
 4;56,57,70,69;,
 4;57,58,71,70;,
 4;58,59,72,71;,
 4;59,60,73,72;,
 4;60,61,74,73;,
 4;61,62,75,74;,
 4;62,63,76,75;,
 4;65,64,77,78;,
 4;64,66,79,77;,
 4;66,67,80,79;,
 4;67,68,81,80;,
 4;68,69,82,81;,
 4;69,70,83,82;,
 4;70,71,84,83;,
 4;71,72,85,84;,
 4;72,73,86,85;,
 4;73,74,87,86;,
 4;74,75,88,87;,
 4;75,76,89,88;,
 4;78,77,90,91;,
 4;77,79,92,90;,
 4;79,80,93,92;,
 4;80,81,94,93;,
 4;81,82,95,94;,
 4;82,83,96,95;,
 4;83,84,97,96;,
 4;84,85,98,97;,
 4;85,86,99,98;,
 4;86,87,100,99;,
 4;87,88,101,100;,
 4;88,89,102,101;,
 4;91,90,103,104;,
 4;90,92,105,103;,
 4;92,93,106,105;,
 4;93,94,107,106;,
 4;94,95,108,107;,
 4;95,96,109,108;,
 4;96,97,110,109;,
 4;97,98,111,110;,
 4;98,99,112,111;,
 4;99,100,113,112;,
 4;100,101,114,113;,
 4;101,102,115,114;,
 4;104,103,116,117;,
 4;103,105,118,116;,
 4;105,106,119,118;,
 4;106,107,120,119;,
 4;107,108,121,120;,
 4;108,109,122,121;,
 4;109,110,123,122;,
 4;110,111,124,123;,
 4;111,112,125,124;,
 4;112,113,126,125;,
 4;113,114,127,126;,
 4;114,115,128,127;,
 4;117,116,129,130;,
 4;116,118,131,129;,
 4;118,119,132,131;,
 4;119,120,133,132;,
 4;120,121,134,133;,
 4;121,122,135,134;,
 4;122,123,136,135;,
 4;123,124,137,136;,
 4;124,125,138,137;,
 4;125,126,139,138;,
 4;126,127,140,139;,
 4;127,128,141,140;,
 4;130,129,142,143;,
 4;129,131,144,142;,
 4;131,132,145,144;,
 4;132,133,146,145;,
 4;133,134,147,146;,
 4;134,135,148,147;,
 4;135,136,149,148;,
 4;136,137,150,149;,
 4;137,138,151,150;,
 4;138,139,152,151;,
 4;139,140,153,152;,
 4;140,141,154,153;,
 3;155,156,157;,
 3;156,158,159;,
 3;158,160,161;,
 3;160,162,163;,
 3;162,164,165;,
 3;164,166,167;,
 3;166,168,169;,
 3;168,170,171;,
 3;170,172,173;,
 3;172,174,175;,
 3;174,176,177;,
 3;176,178,179;;
 
 MeshMaterialList {
  1;
  144;
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
  0,
  0;;
  Material {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;

      TextureFilename {
         "Equirectangular-projection.jpg";
      }
   }
 }
 MeshNormals {
  134;
  -0.000000;1.000000;0.000000;,
  -0.000000;0.966004;-0.258525;,
  0.129263;0.966004;-0.223890;,
  0.223890;0.966004;-0.129263;,
  0.258525;0.966004;0.000000;,
  0.223890;0.966004;0.129263;,
  0.129263;0.966004;0.223890;,
  0.000000;0.966004;0.258525;,
  -0.129263;0.966004;0.223890;,
  -0.223889;0.966004;0.129263;,
  -0.258525;0.966004;0.000000;,
  -0.223890;0.966004;-0.129263;,
  -0.129263;0.966004;-0.223889;,
  -0.000000;0.866285;-0.499550;,
  0.249775;0.866285;-0.432623;,
  0.432623;0.866285;-0.249775;,
  0.499550;0.866285;0.000000;,
  0.432623;0.866285;0.249775;,
  0.249775;0.866285;0.432623;,
  0.000000;0.866285;0.499550;,
  -0.249775;0.866285;0.432623;,
  -0.432623;0.866285;0.249775;,
  -0.499550;0.866285;0.000000;,
  -0.432623;0.866285;-0.249775;,
  -0.249775;0.866285;-0.432623;,
  -0.000000;0.707524;-0.706689;,
  0.353345;0.707524;-0.612011;,
  0.612011;0.707524;-0.353345;,
  0.706689;0.707524;0.000000;,
  0.612011;0.707524;0.353345;,
  0.353345;0.707524;0.612011;,
  0.000000;0.707524;0.706689;,
  -0.353344;0.707524;0.612011;,
  -0.612011;0.707524;0.353345;,
  -0.706689;0.707524;0.000000;,
  -0.612011;0.707524;-0.353344;,
  -0.353345;0.707524;-0.612011;,
  -0.000000;0.500435;-0.865774;,
  0.432887;0.500435;-0.749782;,
  0.749782;0.500435;-0.432887;,
  0.865774;0.500435;0.000000;,
  0.749782;0.500435;0.432887;,
  0.432887;0.500435;0.749782;,
  0.000000;0.500435;0.865774;,
  -0.432887;0.500435;0.749782;,
  -0.749782;0.500435;0.432887;,
  -0.865774;0.500435;0.000000;,
  -0.749782;0.500435;-0.432886;,
  -0.432887;0.500435;-0.749782;,
  -0.000000;0.259096;-0.965852;,
  0.482926;0.259096;-0.836452;,
  0.836452;0.259096;-0.482926;,
  0.965852;0.259096;0.000000;,
  0.836452;0.259096;0.482926;,
  0.482926;0.259096;0.836452;,
  0.000000;0.259096;0.965852;,
  -0.482926;0.259096;0.836452;,
  -0.836452;0.259096;0.482926;,
  -0.965851;0.259096;0.000000;,
  -0.836452;0.259096;-0.482925;,
  -0.482926;0.259096;-0.836452;,
  -0.000000;0.000000;-1.000000;,
  0.500000;0.000000;-0.866025;,
  0.866025;0.000000;-0.500000;,
  1.000000;0.000000;0.000000;,
  0.866025;0.000000;0.500000;,
  0.500000;0.000000;0.866025;,
  0.000000;0.000000;1.000000;,
  -0.500000;0.000000;0.866026;,
  -0.866025;0.000000;0.500000;,
  -1.000000;0.000000;0.000000;,
  -0.866026;0.000000;-0.499999;,
  -0.500000;0.000000;-0.866025;,
  -0.000000;-0.259096;-0.965852;,
  0.482926;-0.259096;-0.836452;,
  0.836452;-0.259096;-0.482926;,
  0.965852;-0.259096;0.000000;,
  0.836452;-0.259096;0.482926;,
  0.482926;-0.259096;0.836452;,
  0.000000;-0.259096;0.965852;,
  -0.482926;-0.259096;0.836452;,
  -0.836452;-0.259096;0.482926;,
  -0.965851;-0.259096;0.000000;,
  -0.836452;-0.259096;-0.482925;,
  -0.482926;-0.259096;-0.836452;,
  -0.000000;-0.500435;-0.865774;,
  0.432887;-0.500435;-0.749782;,
  0.749782;-0.500435;-0.432887;,
  0.865774;-0.500435;0.000000;,
  0.749782;-0.500435;0.432887;,
  0.432887;-0.500435;0.749782;,
  0.000000;-0.500435;0.865774;,
  -0.432887;-0.500435;0.749782;,
  -0.749782;-0.500435;0.432887;,
  -0.865774;-0.500435;0.000000;,
  -0.749782;-0.500435;-0.432887;,
  -0.432887;-0.500435;-0.749782;,
  -0.000000;-0.707524;-0.706689;,
  0.353345;-0.707524;-0.612011;,
  0.612011;-0.707524;-0.353345;,
  0.706689;-0.707524;0.000000;,
  0.612011;-0.707524;0.353345;,
  0.353345;-0.707524;0.612011;,
  0.000000;-0.707524;0.706689;,
  -0.353344;-0.707524;0.612011;,
  -0.612011;-0.707524;0.353345;,
  -0.706689;-0.707524;0.000000;,
  -0.612011;-0.707524;-0.353344;,
  -0.353345;-0.707524;-0.612011;,
  -0.000000;-0.866285;-0.499550;,
  0.249775;-0.866285;-0.432623;,
  0.432623;-0.866285;-0.249775;,
  0.499550;-0.866285;-0.000000;,
  0.432623;-0.866285;0.249775;,
  0.249775;-0.866285;0.432623;,
  0.000000;-0.866285;0.499550;,
  -0.249775;-0.866285;0.432623;,
  -0.432623;-0.866285;0.249775;,
  -0.499550;-0.866285;0.000000;,
  -0.432623;-0.866285;-0.249775;,
  -0.249775;-0.866285;-0.432623;,
  -0.000000;-0.966004;-0.258526;,
  0.129263;-0.966004;-0.223890;,
  0.223890;-0.966004;-0.129263;,
  0.258526;-0.966004;-0.000000;,
  0.223890;-0.966004;0.129263;,
  0.129263;-0.966004;0.223890;,
  0.000000;-0.966004;0.258526;,
  -0.129263;-0.966004;0.223890;,
  -0.223890;-0.966004;0.129263;,
  -0.258526;-0.966004;0.000000;,
  -0.223890;-0.966004;-0.129263;,
  -0.129263;-0.966004;-0.223890;,
  -0.000000;-1.000000;0.000000;;
  144;
  3;0,2,1;,
  3;0,3,2;,
  3;0,4,3;,
  3;0,5,4;,
  3;0,6,5;,
  3;0,7,6;,
  3;0,8,7;,
  3;0,9,8;,
  3;0,10,9;,
  3;0,11,10;,
  3;0,12,11;,
  3;0,1,12;,
  4;1,2,14,13;,
  4;2,3,15,14;,
  4;3,4,16,15;,
  4;4,5,17,16;,
  4;5,6,18,17;,
  4;6,7,19,18;,
  4;7,8,20,19;,
  4;8,9,21,20;,
  4;9,10,22,21;,
  4;10,11,23,22;,
  4;11,12,24,23;,
  4;12,1,13,24;,
  4;13,14,26,25;,
  4;14,15,27,26;,
  4;15,16,28,27;,
  4;16,17,29,28;,
  4;17,18,30,29;,
  4;18,19,31,30;,
  4;19,20,32,31;,
  4;20,21,33,32;,
  4;21,22,34,33;,
  4;22,23,35,34;,
  4;23,24,36,35;,
  4;24,13,25,36;,
  4;25,26,38,37;,
  4;26,27,39,38;,
  4;27,28,40,39;,
  4;28,29,41,40;,
  4;29,30,42,41;,
  4;30,31,43,42;,
  4;31,32,44,43;,
  4;32,33,45,44;,
  4;33,34,46,45;,
  4;34,35,47,46;,
  4;35,36,48,47;,
  4;36,25,37,48;,
  4;37,38,50,49;,
  4;38,39,51,50;,
  4;39,40,52,51;,
  4;40,41,53,52;,
  4;41,42,54,53;,
  4;42,43,55,54;,
  4;43,44,56,55;,
  4;44,45,57,56;,
  4;45,46,58,57;,
  4;46,47,59,58;,
  4;47,48,60,59;,
  4;48,37,49,60;,
  4;49,50,62,61;,
  4;50,51,63,62;,
  4;51,52,64,63;,
  4;52,53,65,64;,
  4;53,54,66,65;,
  4;54,55,67,66;,
  4;55,56,68,67;,
  4;56,57,69,68;,
  4;57,58,70,69;,
  4;58,59,71,70;,
  4;59,60,72,71;,
  4;60,49,61,72;,
  4;61,62,74,73;,
  4;62,63,75,74;,
  4;63,64,76,75;,
  4;64,65,77,76;,
  4;65,66,78,77;,
  4;66,67,79,78;,
  4;67,68,80,79;,
  4;68,69,81,80;,
  4;69,70,82,81;,
  4;70,71,83,82;,
  4;71,72,84,83;,
  4;72,61,73,84;,
  4;73,74,86,85;,
  4;74,75,87,86;,
  4;75,76,88,87;,
  4;76,77,89,88;,
  4;77,78,90,89;,
  4;78,79,91,90;,
  4;79,80,92,91;,
  4;80,81,93,92;,
  4;81,82,94,93;,
  4;82,83,95,94;,
  4;83,84,96,95;,
  4;84,73,85,96;,
  4;85,86,98,97;,
  4;86,87,99,98;,
  4;87,88,100,99;,
  4;88,89,101,100;,
  4;89,90,102,101;,
  4;90,91,103,102;,
  4;91,92,104,103;,
  4;92,93,105,104;,
  4;93,94,106,105;,
  4;94,95,107,106;,
  4;95,96,108,107;,
  4;96,85,97,108;,
  4;97,98,110,109;,
  4;98,99,111,110;,
  4;99,100,112,111;,
  4;100,101,113,112;,
  4;101,102,114,113;,
  4;102,103,115,114;,
  4;103,104,116,115;,
  4;104,105,117,116;,
  4;105,106,118,117;,
  4;106,107,119,118;,
  4;107,108,120,119;,
  4;108,97,109,120;,
  4;109,110,122,121;,
  4;110,111,123,122;,
  4;111,112,124,123;,
  4;112,113,125,124;,
  4;113,114,126,125;,
  4;114,115,127,126;,
  4;115,116,128,127;,
  4;116,117,129,128;,
  4;117,118,130,129;,
  4;118,119,131,130;,
  4;119,120,132,131;,
  4;120,109,121,132;,
  3;121,122,133;,
  3;122,123,133;,
  3;123,124,133;,
  3;124,125,133;,
  3;125,126,133;,
  3;126,127,133;,
  3;127,128,133;,
  3;128,129,133;,
  3;129,130,133;,
  3;130,131,133;,
  3;131,132,133;,
  3;132,121,133;;
 }
 MeshTextureCoords {
  180;
  0.041667;0.000000;,
  0.083333;0.083333;,
  0.000000;0.083333;,
  0.125000;0.000000;,
  0.166667;0.083333;,
  0.208333;0.000000;,
  0.250000;0.083333;,
  0.291667;0.000000;,
  0.333333;0.083333;,
  0.375000;0.000000;,
  0.416667;0.083333;,
  0.458333;0.000000;,
  0.500000;0.083333;,
  0.541667;0.000000;,
  0.583333;0.083333;,
  0.625000;0.000000;,
  0.666667;0.083333;,
  0.708333;0.000000;,
  0.750000;0.083333;,
  0.791667;0.000000;,
  0.833333;0.083333;,
  0.875000;0.000000;,
  0.916667;0.083333;,
  0.958333;0.000000;,
  1.000000;0.083333;,
  0.083333;0.166667;,
  0.000000;0.166667;,
  0.166667;0.166667;,
  0.250000;0.166667;,
  0.333333;0.166667;,
  0.416667;0.166667;,
  0.500000;0.166667;,
  0.583333;0.166667;,
  0.666667;0.166667;,
  0.750000;0.166667;,
  0.833333;0.166667;,
  0.916667;0.166667;,
  1.000000;0.166667;,
  0.083333;0.250000;,
  0.000000;0.250000;,
  0.166667;0.250000;,
  0.250000;0.250000;,
  0.333333;0.250000;,
  0.416667;0.250000;,
  0.500000;0.250000;,
  0.583333;0.250000;,
  0.666667;0.250000;,
  0.750000;0.250000;,
  0.833333;0.250000;,
  0.916667;0.250000;,
  1.000000;0.250000;,
  0.083333;0.333333;,
  0.000000;0.333333;,
  0.166667;0.333333;,
  0.250000;0.333333;,
  0.333333;0.333333;,
  0.416667;0.333333;,
  0.500000;0.333333;,
  0.583333;0.333333;,
  0.666667;0.333333;,
  0.750000;0.333333;,
  0.833333;0.333333;,
  0.916667;0.333333;,
  1.000000;0.333333;,
  0.083333;0.416667;,
  0.000000;0.416667;,
  0.166667;0.416667;,
  0.250000;0.416667;,
  0.333333;0.416667;,
  0.416667;0.416667;,
  0.500000;0.416667;,
  0.583333;0.416667;,
  0.666667;0.416667;,
  0.750000;0.416667;,
  0.833333;0.416667;,
  0.916667;0.416667;,
  1.000000;0.416667;,
  0.083333;0.500000;,
  0.000000;0.500000;,
  0.166667;0.500000;,
  0.250000;0.500000;,
  0.333333;0.500000;,
  0.416667;0.500000;,
  0.500000;0.500000;,
  0.583333;0.500000;,
  0.666667;0.500000;,
  0.750000;0.500000;,
  0.833333;0.500000;,
  0.916667;0.500000;,
  1.000000;0.500000;,
  0.083333;0.583333;,
  0.000000;0.583333;,
  0.166667;0.583333;,
  0.250000;0.583333;,
  0.333333;0.583333;,
  0.416667;0.583333;,
  0.500000;0.583333;,
  0.583333;0.583333;,
  0.666667;0.583333;,
  0.750000;0.583333;,
  0.833333;0.583333;,
  0.916667;0.583333;,
  1.000000;0.583333;,
  0.083333;0.666667;,
  0.000000;0.666667;,
  0.166667;0.666667;,
  0.250000;0.666667;,
  0.333333;0.666667;,
  0.416667;0.666667;,
  0.500000;0.666667;,
  0.583333;0.666667;,
  0.666667;0.666667;,
  0.750000;0.666667;,
  0.833333;0.666667;,
  0.916667;0.666667;,
  1.000000;0.666667;,
  0.083333;0.750000;,
  0.000000;0.750000;,
  0.166667;0.750000;,
  0.250000;0.750000;,
  0.333333;0.750000;,
  0.416667;0.750000;,
  0.500000;0.750000;,
  0.583333;0.750000;,
  0.666667;0.750000;,
  0.750000;0.750000;,
  0.833333;0.750000;,
  0.916667;0.750000;,
  1.000000;0.750000;,
  0.083333;0.833333;,
  0.000000;0.833333;,
  0.166667;0.833333;,
  0.250000;0.833333;,
  0.333333;0.833333;,
  0.416667;0.833333;,
  0.500000;0.833333;,
  0.583333;0.833333;,
  0.666667;0.833333;,
  0.750000;0.833333;,
  0.833333;0.833333;,
  0.916667;0.833333;,
  1.000000;0.833333;,
  0.083333;0.916667;,
  0.000000;0.916667;,
  0.166667;0.916667;,
  0.250000;0.916667;,
  0.333333;0.916667;,
  0.416667;0.916667;,
  0.500000;0.916667;,
  0.583333;0.916667;,
  0.666667;0.916667;,
  0.750000;0.916667;,
  0.833333;0.916667;,
  0.916667;0.916667;,
  1.000000;0.916667;,
  0.000000;0.916667;,
  0.083333;0.916667;,
  0.041667;1.000000;,
  0.166667;0.916667;,
  0.125000;1.000000;,
  0.250000;0.916667;,
  0.208333;1.000000;,
  0.333333;0.916667;,
  0.291667;1.000000;,
  0.416667;0.916667;,
  0.375000;1.000000;,
  0.500000;0.916667;,
  0.458333;1.000000;,
  0.583333;0.916667;,
  0.541667;1.000000;,
  0.666667;0.916667;,
  0.625000;1.000000;,
  0.750000;0.916667;,
  0.708333;1.000000;,
  0.833333;0.916667;,
  0.791667;1.000000;,
  0.916667;0.916667;,
  0.875000;1.000000;,
  1.000000;0.916667;,
  0.958333;1.000000;;
 }
 MeshVertexColors {
  180;
  0;1.000000;1.000000;1.000000;1.000000;,
  1;1.000000;1.000000;1.000000;1.000000;,
  2;1.000000;1.000000;1.000000;1.000000;,
  3;1.000000;1.000000;1.000000;1.000000;,
  4;1.000000;1.000000;1.000000;1.000000;,
  5;1.000000;1.000000;1.000000;1.000000;,
  6;1.000000;1.000000;1.000000;1.000000;,
  7;1.000000;1.000000;1.000000;1.000000;,
  8;1.000000;1.000000;1.000000;1.000000;,
  9;1.000000;1.000000;1.000000;1.000000;,
  10;1.000000;1.000000;1.000000;1.000000;,
  11;1.000000;1.000000;1.000000;1.000000;,
  12;1.000000;1.000000;1.000000;1.000000;,
  13;1.000000;1.000000;1.000000;1.000000;,
  14;1.000000;1.000000;1.000000;1.000000;,
  15;1.000000;1.000000;1.000000;1.000000;,
  16;1.000000;1.000000;1.000000;1.000000;,
  17;1.000000;1.000000;1.000000;1.000000;,
  18;1.000000;1.000000;1.000000;1.000000;,
  19;1.000000;1.000000;1.000000;1.000000;,
  20;1.000000;1.000000;1.000000;1.000000;,
  21;1.000000;1.000000;1.000000;1.000000;,
  22;1.000000;1.000000;1.000000;1.000000;,
  23;1.000000;1.000000;1.000000;1.000000;,
  24;1.000000;1.000000;1.000000;1.000000;,
  25;1.000000;1.000000;1.000000;1.000000;,
  26;1.000000;1.000000;1.000000;1.000000;,
  27;1.000000;1.000000;1.000000;1.000000;,
  28;1.000000;1.000000;1.000000;1.000000;,
  29;1.000000;1.000000;1.000000;1.000000;,
  30;1.000000;1.000000;1.000000;1.000000;,
  31;1.000000;1.000000;1.000000;1.000000;,
  32;1.000000;1.000000;1.000000;1.000000;,
  33;1.000000;1.000000;1.000000;1.000000;,
  34;1.000000;1.000000;1.000000;1.000000;,
  35;1.000000;1.000000;1.000000;1.000000;,
  36;1.000000;1.000000;1.000000;1.000000;,
  37;1.000000;1.000000;1.000000;1.000000;,
  38;1.000000;1.000000;1.000000;1.000000;,
  39;1.000000;1.000000;1.000000;1.000000;,
  40;1.000000;1.000000;1.000000;1.000000;,
  41;1.000000;1.000000;1.000000;1.000000;,
  42;1.000000;1.000000;1.000000;1.000000;,
  43;1.000000;1.000000;1.000000;1.000000;,
  44;1.000000;1.000000;1.000000;1.000000;,
  45;1.000000;1.000000;1.000000;1.000000;,
  46;1.000000;1.000000;1.000000;1.000000;,
  47;1.000000;1.000000;1.000000;1.000000;,
  48;1.000000;1.000000;1.000000;1.000000;,
  49;1.000000;1.000000;1.000000;1.000000;,
  50;1.000000;1.000000;1.000000;1.000000;,
  51;1.000000;1.000000;1.000000;1.000000;,
  52;1.000000;1.000000;1.000000;1.000000;,
  53;1.000000;1.000000;1.000000;1.000000;,
  54;1.000000;1.000000;1.000000;1.000000;,
  55;1.000000;1.000000;1.000000;1.000000;,
  56;1.000000;1.000000;1.000000;1.000000;,
  57;1.000000;1.000000;1.000000;1.000000;,
  58;1.000000;1.000000;1.000000;1.000000;,
  59;1.000000;1.000000;1.000000;1.000000;,
  60;1.000000;1.000000;1.000000;1.000000;,
  61;1.000000;1.000000;1.000000;1.000000;,
  62;1.000000;1.000000;1.000000;1.000000;,
  63;1.000000;1.000000;1.000000;1.000000;,
  64;1.000000;1.000000;1.000000;1.000000;,
  65;1.000000;1.000000;1.000000;1.000000;,
  66;1.000000;1.000000;1.000000;1.000000;,
  67;1.000000;1.000000;1.000000;1.000000;,
  68;1.000000;1.000000;1.000000;1.000000;,
  69;1.000000;1.000000;1.000000;1.000000;,
  70;1.000000;1.000000;1.000000;1.000000;,
  71;1.000000;1.000000;1.000000;1.000000;,
  72;1.000000;1.000000;1.000000;1.000000;,
  73;1.000000;1.000000;1.000000;1.000000;,
  74;1.000000;1.000000;1.000000;1.000000;,
  75;1.000000;1.000000;1.000000;1.000000;,
  76;1.000000;1.000000;1.000000;1.000000;,
  77;1.000000;1.000000;1.000000;1.000000;,
  78;1.000000;1.000000;1.000000;1.000000;,
  79;1.000000;1.000000;1.000000;1.000000;,
  80;1.000000;1.000000;1.000000;1.000000;,
  81;1.000000;1.000000;1.000000;1.000000;,
  82;1.000000;1.000000;1.000000;1.000000;,
  83;1.000000;1.000000;1.000000;1.000000;,
  84;1.000000;1.000000;1.000000;1.000000;,
  85;1.000000;1.000000;1.000000;1.000000;,
  86;1.000000;1.000000;1.000000;1.000000;,
  87;1.000000;1.000000;1.000000;1.000000;,
  88;1.000000;1.000000;1.000000;1.000000;,
  89;1.000000;1.000000;1.000000;1.000000;,
  90;1.000000;1.000000;1.000000;1.000000;,
  91;1.000000;1.000000;1.000000;1.000000;,
  92;1.000000;1.000000;1.000000;1.000000;,
  93;1.000000;1.000000;1.000000;1.000000;,
  94;1.000000;1.000000;1.000000;1.000000;,
  95;1.000000;1.000000;1.000000;1.000000;,
  96;1.000000;1.000000;1.000000;1.000000;,
  97;1.000000;1.000000;1.000000;1.000000;,
  98;1.000000;1.000000;1.000000;1.000000;,
  99;1.000000;1.000000;1.000000;1.000000;,
  100;1.000000;1.000000;1.000000;1.000000;,
  101;1.000000;1.000000;1.000000;1.000000;,
  102;1.000000;1.000000;1.000000;1.000000;,
  103;1.000000;1.000000;1.000000;1.000000;,
  104;1.000000;1.000000;1.000000;1.000000;,
  105;1.000000;1.000000;1.000000;1.000000;,
  106;1.000000;1.000000;1.000000;1.000000;,
  107;1.000000;1.000000;1.000000;1.000000;,
  108;1.000000;1.000000;1.000000;1.000000;,
  109;1.000000;1.000000;1.000000;1.000000;,
  110;1.000000;1.000000;1.000000;1.000000;,
  111;1.000000;1.000000;1.000000;1.000000;,
  112;1.000000;1.000000;1.000000;1.000000;,
  113;1.000000;1.000000;1.000000;1.000000;,
  114;1.000000;1.000000;1.000000;1.000000;,
  115;1.000000;1.000000;1.000000;1.000000;,
  116;1.000000;1.000000;1.000000;1.000000;,
  117;1.000000;1.000000;1.000000;1.000000;,
  118;1.000000;1.000000;1.000000;1.000000;,
  119;1.000000;1.000000;1.000000;1.000000;,
  120;1.000000;1.000000;1.000000;1.000000;,
  121;1.000000;1.000000;1.000000;1.000000;,
  122;1.000000;1.000000;1.000000;1.000000;,
  123;1.000000;1.000000;1.000000;1.000000;,
  124;1.000000;1.000000;1.000000;1.000000;,
  125;1.000000;1.000000;1.000000;1.000000;,
  126;1.000000;1.000000;1.000000;1.000000;,
  127;1.000000;1.000000;1.000000;1.000000;,
  128;1.000000;1.000000;1.000000;1.000000;,
  129;1.000000;1.000000;1.000000;1.000000;,
  130;1.000000;1.000000;1.000000;1.000000;,
  131;1.000000;1.000000;1.000000;1.000000;,
  132;1.000000;1.000000;1.000000;1.000000;,
  133;1.000000;1.000000;1.000000;1.000000;,
  134;1.000000;1.000000;1.000000;1.000000;,
  135;1.000000;1.000000;1.000000;1.000000;,
  136;1.000000;1.000000;1.000000;1.000000;,
  137;1.000000;1.000000;1.000000;1.000000;,
  138;1.000000;1.000000;1.000000;1.000000;,
  139;1.000000;1.000000;1.000000;1.000000;,
  140;1.000000;1.000000;1.000000;1.000000;,
  141;1.000000;1.000000;1.000000;1.000000;,
  142;1.000000;1.000000;1.000000;1.000000;,
  143;1.000000;1.000000;1.000000;1.000000;,
  144;1.000000;1.000000;1.000000;1.000000;,
  145;1.000000;1.000000;1.000000;1.000000;,
  146;1.000000;1.000000;1.000000;1.000000;,
  147;1.000000;1.000000;1.000000;1.000000;,
  148;1.000000;1.000000;1.000000;1.000000;,
  149;1.000000;1.000000;1.000000;1.000000;,
  150;1.000000;1.000000;1.000000;1.000000;,
  151;1.000000;1.000000;1.000000;1.000000;,
  152;1.000000;1.000000;1.000000;1.000000;,
  153;1.000000;1.000000;1.000000;1.000000;,
  154;1.000000;1.000000;1.000000;1.000000;,
  155;1.000000;1.000000;1.000000;1.000000;,
  156;1.000000;1.000000;1.000000;1.000000;,
  157;1.000000;1.000000;1.000000;1.000000;,
  158;1.000000;1.000000;1.000000;1.000000;,
  159;1.000000;1.000000;1.000000;1.000000;,
  160;1.000000;1.000000;1.000000;1.000000;,
  161;1.000000;1.000000;1.000000;1.000000;,
  162;1.000000;1.000000;1.000000;1.000000;,
  163;1.000000;1.000000;1.000000;1.000000;,
  164;1.000000;1.000000;1.000000;1.000000;,
  165;1.000000;1.000000;1.000000;1.000000;,
  166;1.000000;1.000000;1.000000;1.000000;,
  167;1.000000;1.000000;1.000000;1.000000;,
  168;1.000000;1.000000;1.000000;1.000000;,
  169;1.000000;1.000000;1.000000;1.000000;,
  170;1.000000;1.000000;1.000000;1.000000;,
  171;1.000000;1.000000;1.000000;1.000000;,
  172;1.000000;1.000000;1.000000;1.000000;,
  173;1.000000;1.000000;1.000000;1.000000;,
  174;1.000000;1.000000;1.000000;1.000000;,
  175;1.000000;1.000000;1.000000;1.000000;,
  176;1.000000;1.000000;1.000000;1.000000;,
  177;1.000000;1.000000;1.000000;1.000000;,
  178;1.000000;1.000000;1.000000;1.000000;,
  179;1.000000;1.000000;1.000000;1.000000;;
 }
}
