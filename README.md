# 안전사고 예방을 위한 통신기능이 내장된 안전모 프로젝트 

#        <img width="500" alt="2" src= "https://github.com/user-attachments/assets/b6f79172-8c6c-455a-9e41-65d6dcfc3db7">

## 프로젝트 설명

이 프로젝트는 산업 현장에서 발생할 수 있는 안전사고를 예방하기 위해 통신 기능이 내장된 안전모를 개발하는 것입니다. 안전모는 자이로 센서, 환경 센서, 부저, 안테나, 배터리 등의 다양한 부품을 포함하고 있으며, 위급 상황 발생 시 실시간으로 경고를 전달하고, 근로자의 상태를 모니터링할 수 있습니다.

## 주요 기능

- **자이로 센서**: 착용자의 움직임을 감지하여 낙상 사고를 감지합니다.
- **환경 센서**: 대기질을 감지하여 유해 환경을 실시간으로 모니터링합니다.
- **부저**: 사고 발생 시 착용자의 위치와 상태를 알립니다.
- **LoRa 통신**: 장거리 무선 통신을 통해 관리자에게 실시간으로 데이터를 전달합니다.
- **GUI**: PyQt5를 사용한 사용자 인터페이스를 통해 데이터 시각화 및 실시간 모니터링을 제공합니다.

## 설치 및 실행 방법

1. **Arduino 설정**:
    - Arduino 보드에 자이로 센서, 환경 센서, 부저, 안테나, 배터리 등을 연결합니다.
    - 아래의 코드를 Arduino에 업로드합니다.

    ```c
    // 클라이언트 코드
    #include <SPI.h>
    #include <RH_RF95.h>
    // ... 다른 라이브러리 및 설정
    
    void setup() {
        // 핀 모드 설정 및 통신 초기화
    }
    
    void loop() {
        // 센서 데이터 읽기 및 메시지 전송
    }
    ```

2. **서버 설정**:
    - 서버 코드를 아래와 같이 설정합니다.

    ```python
    # 서버 코드 (Python)
    from PyQt5 import QtWidgets
    # ... 필요한 모듈 임포트
    
    class WindowClass(QtWidgets.QMainWindow, form_class):
        def __init__(self):
            # 초기 설정
        def handle_serial_data(self, data):
            # 시리얼 데이터 처리 및 UI 업데이트
    ```

3. **데이터베이스 설정**:
    - MySQL 데이터베이스를 설정하고, 아래의 테이블을 생성합니다.

    ```sql
    CREATE TABLE sensor_data (
        id INT AUTO_INCREMENT PRIMARY KEY,
        sensor_type VARCHAR(50),
        value FLOAT,
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
    ```

4. **웹사이트 설정**:
    - PHP와 MySQL을 사용하여 데이터를 저장하고 시각화하는 웹 페이지를 구축합니다.

5. **실행**:

   <img width="200" alt="2" src= https://github.com/user-attachments/assets/e49c43cf-4b27-4c0f-9ccb-7d3c704c4330>
   
## 프로젝트 구성

1. **지그 설계**:
    - 안전모에 부착될 지그의 디자인을 설계하고 제작합니다. 지그는 센서와 안테나, 배터리 등을 안전모에 견고하게 부착할 수 있도록 합니다.

2. **센서 납땜 및 회로 설계**:
    - 센서와 부품들을 납땜하여 회로를 구성하고, 안정적인 통신을 위한 연결을 확인합니다.

3. **코딩**:
    - Arduino 및 Python을 사용하여 센서 데이터 처리, 통신, GUI 구현 등의 코드를 작성합니다.

4. **테스트 및 디버깅**:
    - 시스템의 모든 기능을 테스트하고, 필요한 경우 디버깅 작업을 진행합니다.

## 기대 효과

- **실시간 모니터링**: 근로자의 상태를 실시간으로 모니터링하여 신속한 대응이 가능합니다.
- **안전성 향상**: 다양한 센서를 통해 낙상 사고 및 유해 환경 감지가 가능하여 사고 발생률을 낮춥니다.
- **다양한 산업 적용**: 건설 현장 뿐만 아니라 실험실, 연구소 등 다양한 산업현장에서 활용 가능합니다.


## 참고 문서

- [산학 캡스톤디자인_TeamKahng_최종보고서(2023).pdf](https://github.com/user-attachments/files/16349359/_TeamKahng_.2023.pdf)
- [아이디어붐](https://ideaboom.net/project/project/view?seq=1437&search_keyword=%EC%95%88%EC%A0%84%EB%AA%A8&data_seq[]=1&data_seq[]=2&data_seq[]=3&data_seq[]=4&data_seq[]=5&data_seq[]=6&data_seq[]=7&data_seq[]=8&data_seq[]=9&data_seq[]=10&data_seq[]=11&data_seq[]=12&data_seq[]=13&order=reg)
- [시연 영상](https://youtu.be/vbhFi2KNafI?si=6dlD7VHBstM9Rb3n)
