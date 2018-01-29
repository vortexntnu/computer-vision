import cv2
import numpy as np
import math

cap = cv2.VideoCapture(0)

params = cv2.SimpleBlobDetector_Params()

params.filterByArea = True
params.minArea = 200
params.maxArea = 2000

params.filterByCircularity = True
params.minCircularity = 0.4
params.maxCircularity = 0.7

circ_tri_low = 0.4
circ_tri_high = 0.65

circ_squ_low = 0.2
circ_squ_high = 0.35

while True:
    _, frame = cap.read()
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    shape = hsv.shape
    #hsv heu sat value
    hsv_lower_blue = np.array([90,90,90])
    hsv_upper_blue = np.array([130,255,255])
    canvas = np.zeros((shape[0],shape[1],3), np.uint8)
    maskb = cv2.inRange(hsv, hsv_lower_blue, hsv_upper_blue)
    
    hsv_lower_red1 = np.array([175,150,150])
    hsv_upper_red1 = np.array([180,255,255])
    hsv_lower_red2 = np.array([0,150,150])
    hsv_upper_red2 = np.array([2,255,255])

    maskr1 = cv2.inRange(hsv, hsv_lower_red1, hsv_upper_red1)
    maskr2 = cv2.inRange(hsv, hsv_lower_red2, hsv_upper_red2)
    maskr = maskr1+maskr2

    mask = maskr+maskb
    try:
        _, cont, _ = cv2.findContours(maskb, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
        contours_area = []
        for con in cont:
            area = cv2.contourArea(con)
            if 100 < area < 1000000:
                contours_area.append(con)
        contours_triangles = []
        contours_squares = []
        for con in contours_area:
            perimeter = cv2.arcLength(con, True)
            area = cv2.contourArea(con)
            if perimeter == 0:
                break
            circularity = 4*math.pi*(area/(perimeter*perimeter))
            if circ_tri_low < circularity < circ_tri_high:
                contours_triangles.append(con)
            if circ_squ_low < circularity < circ_squ_high:
                contours_squares.append(con)


        for con in contours_triangles:
            M = cv2.moments(con)
            center = (int(M["m10"]/M["m00"]),int(M["m01"]/M["m00"]))
            x,y,w,h = cv2.boundingRect(con)

            cv2.rectangle(canvas,(x,y),(x+w,y+h),[0,255,0])
            cv2.rectangle(frame,(x,y),(x+w,y+h),[0,250,255])
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(canvas, 'Blue Triangle',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
            cv2.putText(frame, 'Blue Triangle',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)

        for con in contours_squares:
            M = cv2.moments(con)
            center = (int(M["m10"]/M["m00"]),int(M["m01"]/M["m00"]))
            x,y,w,h = cv2.boundingRect(con)

            cv2.rectangle(canvas,(x,y),(x+w,y+h),[0,255,0])
            cv2.rectangle(frame,(x,y),(x+w,y+h),[0,250,255])
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(canvas, 'Blue Square',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
            cv2.putText(frame, 'Blue Square',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
            

    except(ValueError, ZeroDivisionError):
        pass

    try:
        _, cont, _ = cv2.findContours(maskr, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        contours_area = []
        for con in cont:
            area = cv2.contourArea(con)
            if 50 < area < 1000000:
                contours_area.append(con)
        contours_triangles = []
        contours_squares = []
        for con in contours_area:
            perimeter = cv2.arcLength(con, True)
            area = cv2.contourArea(con)
            if perimeter == 0:
                break
            circularity = 4*math.pi*(area/(perimeter*perimeter))
            if circ_tri_low < circularity < circ_tri_high:
                contours_triangles.append(con)
            if circ_squ_low <= circularity <= circ_squ_high:
                contours_squares.append(con)


        for con in contours_triangles:
            M = cv2.moments(con)
            center = (int(M["m10"]/M["m00"]),int(M["m01"]/M["m00"]))
            x,y,w,h = cv2.boundingRect(con)

            cv2.rectangle(canvas,(x,y),(x+w,y+h),[0,255,0])
            cv2.rectangle(frame,(x,y),(x+w,y+h),[0,250,255])
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(canvas, 'Red Triangle',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
            cv2.putText(frame, 'Red Triangle',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)

        for con in contours_squares:
            M = cv2.moments(con)
            center = (int(M["m10"]/M["m00"]),int(M["m01"]/M["m00"]))
            x,y,w,h = cv2.boundingRect(con)

            cv2.rectangle(canvas,(x,y),(x+w,y+h),[0,255,0])
            cv2.rectangle(frame,(x,y),(x+w,y+h),[0,250,255])
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(canvas, 'Red Square',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
            cv2.putText(frame, 'Red Square',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
            

    except(ValueError, ZeroDivisionError):
        pass


    hsv_canvas = cv2.cvtColor(canvas, cv2.COLOR_BGR2HSV)
    res = cv2.bitwise_and(frame,frame, mask = mask)
    res += canvas
    frame += hsv_canvas
    cv2.imshow('frame', frame)
    cv2.imshow('mask', mask)
    cv2.imshow('hsv_canvas', hsv_canvas)
    cv2.imshow('res', res)

    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break

cv2.destroyAllWindows()
cap.release()