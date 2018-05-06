"Opengl Shadow Rendering-Speed.exe" models/sehir.xml sdsm SehirPth1.xml > results/Sehir-s.txt 
"Opengl Shadow Rendering-Speed.exe" models/sehir.xml sdsm-noLayers SehirPth1.xml > results/Sehir-s.txt 
"Opengl Shadow Rendering-Speed.exe" models/sehir.xml pssm SehirPth1.xml > results/Sehir-p.txt 
"Opengl Shadow Rendering-Speed.exe" models/sehir.xml pssm SehirPth1.xml basic-poisson > results/Sehir-p-p.txt

"Opengl Shadow Rendering-Speed.exe" models/DesertCity.xml sdsm DesertPth1.xml > results/DesertCity-s.txt
"Opengl Shadow Rendering-Speed.exe" models/DesertCity.xml sdsm DesertPth1.xml > results/DesertCity-s.txt
"Opengl Shadow Rendering-Speed.exe" models/DesertCity.xml pssm DesertPth1.xml > results/DesertCity-p.txt
"Opengl Shadow Rendering-Speed.exe" models/DesertCity.xml pssm DesertPth1.xml  basic-poisson > results/DesertCity-p-p.txt

"Opengl Shadow Rendering-Speed.exe" models/City.xml sdsm CityPth1.xml > results/City-s.txt
"Opengl Shadow Rendering-Speed.exe" models/City.xml sdsm-noLayers CityPth1.xml > results/City-s.txt
"Opengl Shadow Rendering-Speed.exe" models/City.xml pssm CityPth1.xml > results/City-p.txt
"Opengl Shadow Rendering-Speed.exe" models/City.xml pssm CityPth1.xml basic-poisson > results/City-p-p.txt