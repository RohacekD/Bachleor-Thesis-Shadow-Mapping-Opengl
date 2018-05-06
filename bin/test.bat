CALL:RunTestForScene "Sehir", "SehirPth1"
CALL:RunTestForScene "DesertCity", "DesertPth1"
CALL:RunTestForScene "City", "CityPth1"
exit 0

:RunTestForScene ::first argument is xml name, second is path
	"Opengl Shadow Rendering-Speed.exe" models/%~1.xml sdsm %~2.xml 				> results/%~1-sdsm.txt 
	"Opengl Shadow Rendering-Speed.exe" models/%~1.xml sdsm-noLayers %~2.xml 		> results/%~1-sdsm-noLayers.txt 
	"Opengl Shadow Rendering-Speed.exe" models/%~1.xml pssm %~2.xml 				> results/%~1-pssm.txt 
	"Opengl Shadow Rendering-Speed.exe" models/%~1.xml sdsm-noLayers %~2.xml 		> results/%~1-pssm-noLayers.txt 
	"Opengl Shadow Rendering-Speed.exe" models/%~1.xml pssm %~2.xml basic-poisson 	> results/%~1-pssm-poisson.txt
EXIT /B 0