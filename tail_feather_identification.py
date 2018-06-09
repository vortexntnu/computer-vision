import cv2
import numpy as np
import math
def displaying_boxes(contours_triangles,contours_squares,color):
    for con in contours_triangles:
        M = cv2.moments(con)
        center = (int(M["m10"]/M["m00"]),int(M["m01"]/M["m00"]))
        x,y,w,h = cv2.boundingRect(con)

        cv2.rectangle(canvas,(x,y),(x+w,y+h),[0,255,0])
        cv2.rectangle(frame_non_blur,(x,y),(x+w,y+h),[0,250,255])
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(canvas, color+' Triangle',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
        cv2.putText(frame_non_blur, color+' Triangle',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)

    for con in contours_squares:
        M = cv2.moments(con)
        center = (int(M["m10"]/M["m00"]),int(M["m01"]/M["m00"]))
        x,y,w,h = cv2.boundingRect(con)
        #cv2.rectangle(canvas,(x,y),(x+w,y+h),[0,255,0])
        #cv2.rectangle(frame_non_blur,(x,y),(x+w,y+h),[0,250,255])
        cv2.drawContours( frame_non_blur, con, -1, (0, 255, 0), 3 )
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(canvas, color+' Rectangle',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
        cv2.putText(frame_non_blur, color+' Rectangle',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)

def sorting_contours(mask,color):
    try:
        _, cont, _ = cv2.findContours(mask, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        contours_area = []
        for con in cont:
            area = cv2.contourArea(con)
            if min_area < area < max_area:
                contours_area.append(con)
        contours_triangles = []
        contours_squares = []
        for con in contours_area:
            perimeter = cv2.arcLength(con, True)
            if perimeter == 0:
                break
            area = cv2.contourArea(con)
            #con = cv2.approxPolyDP(con,0.2*perimeter,True)
            circularity = 4*math.pi*area/(perimeter*perimeter)
            circularity = 1-circularity
            if circ_tri_low < circularity < circ_tri_high:
                contours_triangles.append(con)
            if circ_squ_low < circularity < circ_squ_high:
                contours_squares.append(con)
        displaying_boxes(contours_triangles,contours_squares, color)
    except(ValueError, ZeroDivisionError):
        pass

cap = cv2.VideoCapture(0) #Which camrea


circ_tri_low = 0.4
circ_tri_high = 0.55

circ_squ_low = 0.55
circ_squ_high = 0.8

min_area = 500
max_area =200000
def web_cam_value_storage():
    #Values for web-camera
    hue_blue_low = 100 #Color angle min
    hue_blue_high = 130 #Color angle max
    sat_blue_low = 100 #How smudged/white
    sat_blue_high = 255 #How clear
    val_blue_low = 80 #How dark
    val_blue_high = 255 #How bright

    hue_red_low = 176
    hue_red_high = 3
    sat_red_low = 120
    sat_red_high = 255
    val_red_low = 100
    val_red_high = 255

    hue_yellow_low = 20
    hue_yellow_high = 40
    sat_yellow_low = 100
    sat_yellow_high = 255
    val_yellow_low = 80
    val_yellow_high = 255
    '''
    hue_orange_low = 5
    hue_orange_high = 20
    sat_orange_low = 0
    sat_orange_high = 255
    val_orange_low = 0
    val_orange_high = 255
    '''

#Values for USB-camera
hue_blue_low = 100 #Color angle min
hue_blue_high = 130 #Color angle max
sat_blue_low = 100 #How smudged/white
sat_blue_high = 255 #How clear
val_blue_low = 100 #How dark
val_blue_high = 210 #How bright

hue_red_low = 177
hue_red_high = 8
sat_red_low = 105
sat_red_high = 255
val_red_low = 85
val_red_high = 210

hue_yellow_low = 25
hue_yellow_high = 35
sat_yellow_low = 100
sat_yellow_high = 255
val_yellow_low = 80
val_yellow_high = 210


while True:
    _, frame = cap.read()
    frame_non_blur = frame.copy()
    frame = cv2.GaussianBlur(frame, (15,15), 0)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    shape = hsv.shape
    #hsv heu sat value
    hsv_lower_blue = np.array([hue_blue_low,sat_blue_low,val_blue_low])
    hsv_upper_blue = np.array([hue_blue_high,sat_blue_high,val_blue_high])
    canvas = np.zeros((shape[0],shape[1],3), np.uint8)
    maskb = cv2.inRange(hsv, hsv_lower_blue, hsv_upper_blue)
    
    hsv_lower_red1 = np.array([hue_red_low,sat_red_low,val_red_low])
    hsv_upper_red1 = np.array([180,sat_red_high,val_red_high])
    hsv_lower_red2 = np.array([0,sat_red_low,val_red_low])
    hsv_upper_red2 = np.array([hue_red_high,sat_red_high,val_red_high])

    maskr1 = cv2.inRange(hsv, hsv_lower_red1, hsv_upper_red1)
    maskr2 = cv2.inRange(hsv, hsv_lower_red2, hsv_upper_red2)
    maskr = maskr1+maskr2

    hsv_lower_yellow = np.array([hue_yellow_low,sat_yellow_low,val_yellow_low])
    hsv_upper_yellow = np.array([hue_yellow_high,sat_yellow_high,val_yellow_high])
    canvas = np.zeros((shape[0],shape[1],3), np.uint8)
    masky = cv2.inRange(hsv, hsv_lower_yellow, hsv_upper_yellow)
    '''
    hsv_lower_orange = np.array([hue_orange_low,sat_orange_low,val_orange_low])
    hsv_upper_orange = np.array([hue_orange_high,sat_orange_high,val_orange_high])
    canvas = np.zeros((shape[0],shape[1],3), np.uint8)
    masko = cv2.inRange(hsv, hsv_lower_orange, hsv_upper_orange)
    '''
    mask = maskr+maskb+masky
    sorting_contours(maskb,'Blue')

    sorting_contours(maskr,'Red')

    sorting_contours(masky,'Yellow')

    '''
    try:
        _, cont, _ = cv2.findContours(maskb, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        contours_area = []
        for con in cont:
            area = cv2.contourArea(con)
            if min_area < area < max_area:
                contours_area.append(con)
        contours_triangles = []
        contours_squares = []
        for con in contours_area:
            perimeter = cv2.arcLength(con, True)
            area = cv2.contourArea(con)
            if perimeter == 0:
                break
            circularity = 4*math.pi*area/(perimeter*perimeter)
            circularity = 1-circularity
            if circ_tri_low < circularity < circ_tri_high:
                contours_triangles.append(con)
            if circ_squ_low < circularity < circ_squ_high:
                contours_squares.append(con)

        displayig_boxes(contours_triangles,contours_squares, 'Blue')
    except(ValueError, ZeroDivisionError):
        pass

    try:
        _, cont, _ = cv2.findContours(maskr, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        contours_area = []
        for con in cont:
            area = cv2.contourArea(con)
            if min_area < area < max_area:
                contours_area.append(con)
        contours_triangles = []
        contours_squares = []
        for con in contours_area:
            perimeter = cv2.arcLength(con, True)
            area = cv2.contourArea(con)
            if perimeter == 0:
                break
            circularity = 4*math.pi*(area/(perimeter*perimeter))
            circularity = 1-circularity
            if circ_tri_low < circularity < circ_tri_high:
                contours_triangles.append(con)
            if circ_squ_low <= circularity <= circ_squ_high:
                contours_squares.append(con)

        displayig_boxes(contours_triangles,contours_squares, 'Red')

    except(ValueError, ZeroDivisionError):
        pass

    try:
        _, cont, _ = cv2.findContours(masky, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        contours_area = []
        for con in cont:
            area = cv2.contourArea(con)
            if min_area < area < max_area:
                contours_area.append(con)
        contours_triangles = []
        contours_squares = []
        for con in contours_area:
            perimeter = cv2.arcLength(con, True)
            area = cv2.contourArea(con)
            if perimeter == 0:
                break
            circularity = 4*math.pi*(area/(perimeter*perimeter))
            circularity = 1-circularity
            if circ_tri_low < circularity < circ_tri_high:
                contours_triangles.append(con)
            if circ_squ_low <= circularity <= circ_squ_high:
                contours_squares.append(con)

        displayig_boxes(contours_triangles,contours_squares, 'Yellow')

    except(ValueError, ZeroDivisionError):
        pass
    '''
    '''
    try:
        _, cont, _ = cv2.findContours(masko, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        contours_area = []
        for con in cont:
            area = cv2.contourArea(con)
            if 200 < area < 1000000:
                contours_area.append(con)
        max_area_ging = 200
        ginger = []
        for con in contours_area:
            area = cv2.contourArea(con)
            if area > max_area_ging:
                ginger = con
                max_area_ging = cv2.contourArea(con)
        if len(ginger)>=1:
            M = cv2.moments(ginger)
            center = (int(M["m10"]/M["m00"]),int(M["m01"]/M["m00"]))
            x,y,w,h = cv2.boundingRect(ginger)
            cv2.rectangle(canvas,(x,y),(x+w,y+h),[0,255,0])
            cv2.rectangle(frame_non_blur,(x,y),(x+w,y+h),[0,250,255])
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(frame_non_blur, 'Kill it with fire!',(x,y),font,1,(0,255,0),2,cv2.LINE_AA)
    except(ValueError, ZeroDivisionError):
        pass
    '''

    hsv_canvas = cv2.cvtColor(canvas, cv2.COLOR_BGR2HSV)
    res = cv2.bitwise_and(frame,frame, mask = mask)
    res += canvas
    frame += hsv_canvas
    #cv2.imshow('frame', frame)
    cv2.imshow('mask', mask)
    #cv2.imshow('hsv_canvas', hsv_canvas)
    cv2.imshow('res', res)
    cv2.imshow('frame_non_blur',frame_non_blur)

    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break

cv2.destroyAllWindows()
cap.release()
