# -*- coding: cp1252 -*-
import cv2
from ultralytics import YOLO
import numpy as np
import json
import httpx as requests

model = YOLO('yolov8n.pt')

video_path = "cruzamento.mp4"

cap = cv2.VideoCapture(video_path)

area_rua1 = np.array([[1184, 760], [1683, 744], [1654, 626], [1215, 629]])
area_rua2 = np.array([[12, 971], [14, 598], [593, 789], [1099, 839]])

rlista = {'lista1': 0, 'lista2': 0}

while cap.isOpened():
    
    success, frame = cap.read()

    if success:
        
        results = model(frame, classes=[2,3], conf=0.45)
        
        rlista['lista1'] = 0
        rlista['lista2'] = 0
        
        for result in results:
            for box in result.boxes.xyxy.cpu().numpy():
                x_centro = (box[0] + box[2]) / 2  
                y_centro = (box[1] + box[3]) / 2  

                if cv2.pointPolygonTest(area_rua1, (x_centro, y_centro), False) >= 0:
                    rlista['lista1'] += 1
                elif cv2.pointPolygonTest(area_rua2, (x_centro, y_centro), False) >= 0:
                    rlista['lista2'] += 1

        rlista['lista1'] = [rlista['lista1']]
        rlista['lista2'] = [rlista['lista2']]

        rlista_json = json.dumps(rlista, indent=2)

        response = requests.post("https://testesemaforo.onrender.com/receber_rlista/", json=rlista)
        print(response.status_code)
        print("Response JSON:", response.json())

        annotated_frame = results[0].plot()
        
        cv2.polylines(annotated_frame, [area_rua1], isClosed=True, color=(0, 255, 0), thickness=2)
        cv2.polylines(annotated_frame, [area_rua2], isClosed=True, color=(0, 255, 0), thickness=2)
        cv2.imshow("Inferencia da Yolo", annotated_frame)
        if cv2.waitKey(25) & 0xFF == ord('q'):
            break
        
cap.release()
cv2.destroyAllWindows()