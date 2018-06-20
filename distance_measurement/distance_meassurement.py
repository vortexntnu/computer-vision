import cv2
import numpy as np
click_number = 0
pixelPoints = []
realPoints = np.array([[0,0],[265,0],[0,167],[265,167]])
def click(event, x, y, flags, param):
    global click_number
    global realPoints
    if event == cv2.EVENT_LBUTTONDOWN:
        click_number += 1
        print(click_number)
        pixelPoints.append([x,y])
        print(pixelPoints)
    if click_number == 4:
        click_number = 5
        pixelPoints_np = np.array(pixelPoints, dtype = "float32")

        retval, mask = cv2.findHomography(pixelPoints_np,realPoints)
        warped = cv2.warpPerspective(img, retval, (500,500))
        cv2.namedWindow('warped',cv2.WINDOW_NORMAL)
        cv2.resizeWindow('warped', 600,600)
        cv2.imshow('warped',warped)




img = cv2.imread('distance3.JPG')

cv2.namedWindow('image',cv2.WINDOW_NORMAL)
cv2.resizeWindow('image', 600,600)
cv2.imshow('image',img)

cv2.setMouseCallback("image", click)


k = cv2.waitKey(0)
if k == 27:         # wait for ESC key to exit
    cv2.destroyAllWindows()