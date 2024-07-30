SamacSys ECAD Model
13391133/1411720/2.50/2/2/Capacitor Polarised

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r400_200"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 2.000) (shapeHeight 4.000))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Default"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 50 mils)
			(strokeWidth 5 mils)
		)
	)
	(patternDef "EEETC1V470P" (originalName "EEETC1V470P")
		(multiLayer
			(pad (padNum 1) (padStyleRef r400_200) (pt 0.000, -3.550) (rotation 0))
			(pad (padNum 2) (padStyleRef r400_200) (pt 0.000, 3.550) (rotation 0))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0.000, 0.000) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 30)
			(line (pt -5.15 6.55) (pt 5.15 6.55) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 5.15 6.55) (pt 5.15 -6.55) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 5.15 -6.55) (pt -5.15 -6.55) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt -5.15 -6.55) (pt -5.15 6.55) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -4.15 -3) (pt -4.15 4.15) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -4.15 4.15) (pt 4.15 4.15) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 4.15 4.15) (pt 4.15 -3) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 4.15 -3) (pt 2.075 -4.15) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.075 -4.15) (pt -2.075 -4.15) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.075 -4.15) (pt -4.15 -3) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.5 -4.15) (pt -2.075 -4.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -2.075 -4.15) (pt -4.15 -3) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -4.15 -3) (pt -4.15 4.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -4.15 4.15) (pt -1.5 4.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 1.5 4.15) (pt 4.15 4.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 4.15 4.15) (pt 4.15 -3) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 4.15 -3) (pt 2.075 -4.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 2.075 -4.15) (pt 1.5 -4.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 0 -6.1) (pt 0 -6.1) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt 0, -6.15) (radius 0.05) (startAngle 90.0) (sweepAngle 180.0) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 0 -6.2) (pt 0 -6.2) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt 0, -6.15) (radius 0.05) (startAngle 270) (sweepAngle 180.0) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 0 -6.1) (pt 0 -6.1) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt 0, -6.15) (radius 0.05) (startAngle 90.0) (sweepAngle 180.0) (width 0.2))
		)
	)
	(symbolDef "EEE-FP0J102AL" (originalName "EEE-FP0J102AL")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 100 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 0 mils -35 mils) (rotation 0]) (justify "UpperLeft") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 500 mils 0 mils) (rotation 180) (pinLength 100 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 500 mils -35 mils) (rotation 0]) (justify "UpperRight") (textStyleRef "Default"))
		))
		(line (pt 200 mils 100 mils) (pt 200 mils -100 mils) (width 6 mils))
		(line (pt 200 mils -100 mils) (pt 230 mils -100 mils) (width 6 mils))
		(line (pt 230 mils -100 mils) (pt 230 mils 100 mils) (width 6 mils))
		(line (pt 230 mils 100 mils) (pt 200 mils 100 mils) (width 6 mils))
		(line (pt 180 mils 50 mils) (pt 140 mils 50 mils) (width 6 mils))
		(line (pt 160 mils 70 mils) (pt 160 mils 30 mils) (width 6 mils))
		(line (pt 100 mils 0 mils) (pt 200 mils 0 mils) (width 6 mils))
		(line (pt 300 mils 0 mils) (pt 400 mils 0 mils) (width 6 mils))
		(poly (pt 300 mils 100 mils) (pt 300 mils -100 mils) (pt 270 mils -100 mils) (pt 270 mils 100 mils) (pt 300 mils 100 mils) (width 10  mils))
		(attr "RefDes" "RefDes" (pt 350 mils 250 mils) (justify 24) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "EEE-FP0J102AL" (originalName "EEE-FP0J102AL") (compHeader (numPins 2) (numParts 1) (refDesPrefix C)
		)
		(compPin "1" (pinName "+") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "-") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "EEE-FP0J102AL"))
		(attachedPattern (patternNum 1) (patternName "EEETC1V470P")
			(numPads 2)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
			)
		)
		(attr "Mouser Part Number" "667-EEE-FP0J102AL")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Panasonic/EEE-FP0J102AL?qs=vmHwEFxEFR%252Bryl7N1%2FXlYg%3D%3D")
		(attr "Manufacturer_Name" "Panasonic")
		(attr "Manufacturer_Part_Number" "EEE-FP0J102AL")
		(attr "Description" "Aluminium Electroyltic Capacitor, Radial")
		(attr "Datasheet Link" "http://industrial.panasonic.com/cdbs/www-data/pdf/RDE0000/RDE0000C1272.pdf")
	)

)
