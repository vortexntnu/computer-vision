import cv2
import numpy as np


pixelPoints = np.array([[286, 2734],[2785, 2766],[319,1192],[2780,1209]], dtype = "float32")

realPoints = np.array([[0,0],[265,0],[0,167],[265,167]])

img = cv2.imread('distance3.JPG')
print(pixelPoints)
print(realPoints)

retval, mask = cv2.findHomography(pixelPoints,realPoints)

print(retval)

warped = cv2.warpPerspective(img, retval, (500,500)) 




cv2.namedWindow('image',cv2.WINDOW_NORMAL)
cv2.resizeWindow('image', 600,600)
cv2.imshow('image',img)

cv2.namedWindow('warped',cv2.WINDOW_NORMAL)
cv2.resizeWindow('warped', 600,600)
cv2.imshow('warped',warped)


k = cv2.waitKey(0)
if k == 27:         # wait for ESC key to exit
    cv2.destroyAllWindows()