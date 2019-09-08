import cv2
import numpy as np
import sys
import os
import dlib
import random


datasets = 'datasets'
name = 'Haotian2'
path = os.path.join(datasets, name)
if not os.path.isdir(path):
    os.mkdir(path)

faceCascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")


video_capture = cv2.VideoCapture(0)
detector = dlib.get_frontal_face_detector()


counter = 1225
def relight(img, light=1, bias=0):
    w = img.shape[1]
    h = img.shape[0]
    #image = []
    for i in range(0,w):
        for j in range(0,h):
            for c in range(3):
                tmp = int(img[j,i,c]*light + bias)
                if tmp > 255:
                    tmp = 255
                elif tmp < 0:
                    tmp = 0
                img[j,i,c] = tmp
    return img
while True:
    # Capture frame-by-frame
    ret, frame = video_capture.read()

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
##################dlib#################################

    # The 1 in the second argument indicates that we should upsample the image
    # 1 time.  This will make everything bigger and allow us to detect more
    # faces.
    dets = detector(gray, 1)
    for i, d in enumerate(dets):
        t = d.top() if d.top() > 0 else 0
        b = d.bottom() if d.bottom() > 0 else 0
        l = d.left() if d.left() > 0 else 0
        r = d.right() if d.right() > 0 else 0
        print("Detection {}: Left: {} Top: {} Right: {} Bottom: {}".format(
            counter, d.left(), d.top(), d.right(), d.bottom()))
        face = frame[t:b, l:r] 
        face = relight(face, random.uniform(0.5, 1.5), random.randint(-60, 60))
        face_resize = cv2.resize(face, (100,100))
        cv2.imwrite('% s/% s***% s.png' % (path, name, counter), face_resize)
        cv2.rectangle(frame, (l, t), (r, b), (0,255,0), 2)
        counter += 1
##################dlib#################################

##################CV2##################################    
    # faces = faceCascade.detectMultiScale(
    #     gray,
    #     scaleFactor=1.1,
    #     minNeighbors=5,
    #     minSize=(100, 100),
    #     flags=cv2.CASCADE_SCALE_IMAGE
    # )
    # # print(faces)
    # # Draw a rectangle around the faces
    # for (x, y, w, h) in faces:
    #     cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
    #     face = gray[y:y+h, x:x+w]
    #     face_resize = cv2.resize(face, (100,100))
    #     # print(face)
    #     cv2.imwrite('% s/name_% s.png' % (path, counter), face_resize)
    #     counter += 1
    # # Display the resulting frame
##################CV2#################################
    if(counter == 12000):
        break
    cv2.imshow('Video', frame)

    if cv2.waitKey(2) & 0xFF == ord('q'):
        break

# When everything is done, release the capture
video_capture.release()
cv2.destroyAllWindows()
