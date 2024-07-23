SamacSys ECAD Model
1627014/1411720/2.50/3/4/Switch 2-Pin Push

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r160_100"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 1.000) (shapeHeight 1.600))
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
	(patternDef "CAS120A1" (originalName "CAS120A1")
		(multiLayer
			(pad (padNum 1) (padStyleRef r160_100) (pt -1.750, -1.150) (rotation 0))
			(pad (padNum 2) (padStyleRef r160_100) (pt 0.000, 1.150) (rotation 0))
			(pad (padNum 3) (padStyleRef r160_100) (pt 1.750, -1.150) (rotation 0))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0.000, 0.000) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.7 1.25) (pt 2.7 1.25) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.7 1.25) (pt 2.7 -1.25) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.7 -1.25) (pt -2.7 -1.25) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.7 -1.25) (pt -2.7 1.25) (width 0.2))
		)
		(layerContents (layerNumRef 30)
			(line (pt -3.7 2.95) (pt 3.7 2.95) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 3.7 2.95) (pt 3.7 -2.95) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 3.7 -2.95) (pt -3.7 -2.95) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt -3.7 -2.95) (pt -3.7 2.95) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -2.7 -1.15) (pt -2.7 -1.15) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -2.7 -1.15) (pt -2.7 1.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -2.7 1.25) (pt -1 1.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 1 1.25) (pt 2.7 1.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 2.7 1.25) (pt 2.7 -1) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.5 -1.25) (pt 0.5 -1.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.7 -2.5) (pt -1.7 -2.5) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -1.7, -2.6) (radius 0.1) (startAngle 90.0) (sweepAngle 180.0) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.7 -2.7) (pt -1.7 -2.7) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -1.7, -2.6) (radius 0.1) (startAngle 270) (sweepAngle 180.0) (width 0.2))
		)
	)
	(symbolDef "CAS-120A1" (originalName "CAS-120A1")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 1000 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 770 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 3) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(line (pt 200 mils 100 mils) (pt 800 mils 100 mils) (width 6 mils))
		(line (pt 800 mils 100 mils) (pt 800 mils -200 mils) (width 6 mils))
		(line (pt 800 mils -200 mils) (pt 200 mils -200 mils) (width 6 mils))
		(line (pt 200 mils -200 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 850 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "CAS-120A1" (originalName "CAS-120A1") (compHeader (numPins 3) (numParts 1) (refDesPrefix S)
		)
		(compPin "1" (pinName "NO") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "COM") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "3" (pinName "NC") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "CAS-120A1"))
		(attachedPattern (patternNum 1) (patternName "CAS120A1")
			(numPads 3)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
			)
		)
		(attr "Mouser Part Number" "229-CAS-120A1")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Nidec-Components/CAS-120A1?qs=XeJtXLiO41SLHVe%252BE1BF5w%3D%3D")
		(attr "Manufacturer_Name" "Nidec Copal")
		(attr "Manufacturer_Part_Number" "CAS-120A1")
		(attr "Description" "Surface Mount Slide Switch Single Pole Double Throw (SPDT) 100 (Non-Switching) mA, 100 (Switching) mA Slide")
		(attr "Datasheet Link" "https://www.nidec-copal-electronics.com/e/catalog/switch/cas.pdf")
		(attr "Height" "2.5 mm")
	)

)
