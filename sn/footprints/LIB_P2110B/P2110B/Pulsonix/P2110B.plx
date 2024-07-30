PULSONIX_LIBRARY_ASCII "SamacSys ECAD Model"
//1329060/1411720/2.50/14/4/Power Supply

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r190.5_127"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 1.270) (shapeHeight 1.905))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Normal"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 1.27)
			(strokeWidth 0.127)
		)
	)
	(patternDef "P2110B" (originalName "P2110B")
		(multiLayer
			(pad (padNum 1) (padStyleRef r190.5_127) (pt -6.985, 4.953) (rotation 90))
			(pad (padNum 2) (padStyleRef r190.5_127) (pt -6.985, 3.302) (rotation 90))
			(pad (padNum 3) (padStyleRef r190.5_127) (pt -6.985, 1.651) (rotation 90))
			(pad (padNum 4) (padStyleRef r190.5_127) (pt -6.985, 0.000) (rotation 90))
			(pad (padNum 5) (padStyleRef r190.5_127) (pt -6.985, -1.651) (rotation 90))
			(pad (padNum 6) (padStyleRef r190.5_127) (pt -6.985, -3.302) (rotation 90))
			(pad (padNum 7) (padStyleRef r190.5_127) (pt -6.985, -4.953) (rotation 90))
			(pad (padNum 8) (padStyleRef r190.5_127) (pt 6.985, -4.953) (rotation 90))
			(pad (padNum 9) (padStyleRef r190.5_127) (pt 6.985, -3.302) (rotation 90))
			(pad (padNum 10) (padStyleRef r190.5_127) (pt 6.985, -1.651) (rotation 90))
			(pad (padNum 11) (padStyleRef r190.5_127) (pt 6.985, 0.000) (rotation 90))
			(pad (padNum 12) (padStyleRef r190.5_127) (pt 6.985, 1.651) (rotation 90))
			(pad (padNum 13) (padStyleRef r190.5_127) (pt 6.985, 3.302) (rotation 90))
			(pad (padNum 14) (padStyleRef r190.5_127) (pt 6.985, 4.953) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt -0.431, -0.000) (textStyleRef "Normal") (isVisible True))
		)
		(layerContents (layerNumRef 28)
			(line (pt -6.985 6.731) (pt 6.985 6.731) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 6.985 6.731) (pt 6.985 -6.731) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 6.985 -6.731) (pt -6.985 -6.731) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -6.985 -6.731) (pt -6.985 6.731) (width 0.025))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -9.3 7.231) (pt 8.438 7.231) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 8.438 7.231) (pt 8.438 -7.231) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 8.438 -7.231) (pt -9.3 -7.231) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -9.3 -7.231) (pt -9.3 7.231) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -7 6) (pt -6.985 6.731) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6.985 6.731) (pt 7 6.731) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 7 6.731) (pt 7 6) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -7 -6) (pt -7 -6.731) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -7 -6.731) (pt 6.985 -6.731) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 6.985 -6.731) (pt 6.985 -6) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -8.7 5.1) (pt -8.7 5.1) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -8.7, 5) (radius 0.1) (startAngle 90.0) (sweepAngle -180.0) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -8.7 4.9) (pt -8.7 4.9) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -8.7, 5) (radius 0.1) (startAngle 270) (sweepAngle -180.0) (width 0.2))
		)
	)
	(symbolDef "P2110B" (originalName "P2110B")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 2) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 3) (pt 0 mils -200 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -225 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 4) (pt 0 mils -300 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -325 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 5) (pt 0 mils -400 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -425 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 6) (pt 0 mils -500 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -525 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 7) (pt 0 mils -600 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -625 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 8) (pt 1200 mils -600 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -625 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 9) (pt 1200 mils -500 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -525 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 10) (pt 1200 mils -400 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -425 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 11) (pt 1200 mils -300 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -325 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 12) (pt 1200 mils -200 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -225 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 13) (pt 1200 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 14) (pt 1200 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(line (pt 200 mils 100 mils) (pt 1000 mils 100 mils) (width 6 mils))
		(line (pt 1000 mils 100 mils) (pt 1000 mils -700 mils) (width 6 mils))
		(line (pt 1000 mils -700 mils) (pt 200 mils -700 mils) (width 6 mils))
		(line (pt 200 mils -700 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 1050 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Normal"))
		(attr "Type" "Type" (pt 1050 mils 200 mils) (justify Left) (isVisible True) (textStyleRef "Normal"))

	)
	(compDef "P2110B" (originalName "P2110B") (compHeader (numPins 14) (numParts 1) (refDesPrefix PS)
		)
		(compPin "1" (pinName "NC_1") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "2" (pinName "GND_1") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "3" (pinName "RFIN") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "4" (pinName "GND_2") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "5" (pinName "DSET") (partNum 1) (symPinNum 5) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "6" (pinName "INT") (partNum 1) (symPinNum 6) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "7" (pinName "RESET") (partNum 1) (symPinNum 7) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "8" (pinName "VCAP") (partNum 1) (symPinNum 8) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "9" (pinName "NC_2") (partNum 1) (symPinNum 9) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "10" (pinName "VSET") (partNum 1) (symPinNum 10) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "11" (pinName "GND_3") (partNum 1) (symPinNum 11) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "12" (pinName "VOUT") (partNum 1) (symPinNum 12) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "13" (pinName "DOUT") (partNum 1) (symPinNum 13) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "14" (pinName "NC_3") (partNum 1) (symPinNum 14) (gateEq 0) (pinEq 0) (pinType Unknown))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "P2110B"))
		(attachedPattern (patternNum 1) (patternName "P2110B")
			(numPads 14)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
				(padNum 4) (compPinRef "4")
				(padNum 5) (compPinRef "5")
				(padNum 6) (compPinRef "6")
				(padNum 7) (compPinRef "7")
				(padNum 8) (compPinRef "8")
				(padNum 9) (compPinRef "9")
				(padNum 10) (compPinRef "10")
				(padNum 11) (compPinRef "11")
				(padNum 12) (compPinRef "12")
				(padNum 13) (compPinRef "13")
				(padNum 14) (compPinRef "14")
			)
		)
		(attr "Mouser Part Number" "582-P2110B")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Powercast/P2110B?qs=VAB4DzKv5wViaDUvcUIcdQ%3D%3D")
		(attr "Manufacturer_Name" "Powercast")
		(attr "Manufacturer_Part_Number" "P2110B")
		(attr "Description" "Energy Harvesting Modules Powerharvester Receiver - 915 MHz")
		(attr "<Hyperlink>" "https://componentsearchengine.com/Datasheets/1/P2110B.pdf")
		(attr "<Component Height>" "2.3114")
		(attr "<STEP Filename>" "P2110B.stp")
		(attr "<STEP Offsets>" "X=0;Y=0;Z=0")
		(attr "<STEP Rotation>" "X=0;Y=0;Z=0")
	)

)
