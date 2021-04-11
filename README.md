# 전자공학과 돌쇠 2학기 세미나- 임베디드 프로그래밍을 이용한 라인트레이서 만들기 

과동아리인 돌쇠에서는 1학년 때 4번의 세미나를 합니다. 매주 만나서 1학기 때는 파이썬, C++프로그래밍을 배웠고 2학기 때는 다양한 소자를 연결해서 시계를 만드는 세미나와 라인트레이서 세미나를 했습니다. 한 달동안 아래 회로에다 알아서 프로그래밍을 해 넣어 라인을 가장 잘 따라가는 자동차를 만드는 것입니다. 

![SE-af529450-6da5-4374-86f4-d48e314e3327 (1)](https://user-images.githubusercontent.com/52185595/114294762-a6b00180-9adb-11eb-95c9-030c4f3702ef.png)

위 사진의 기판을 사용했습니다. 세미나는 다음과 같은 과정으로 진행됩니다. 

> 기판에 납땜하기 -> keil, stlink 다운받기 -> 프로그래밍하기(c++)

트랙에는 스쿨존 (세 줄로 표시된 두 지점 사이는 느리게 운행해야 함) , 정지선 등 미션이 포함되어 있습니다. 

## 납땜하기

임베디드 프로그래밍이란 특정 목적을 가지고 프로그래밍을 하는 것을 말합니다. 

![스크린샷_2020-01-06_오전_10 37 20](https://user-images.githubusercontent.com/52185595/114294844-35248300-9adc-11eb-9e8b-c641b6921dca.png)

위의 회로도를 보고 납땜을 진행했습니다. 

## 카메라 필터에 관하여(노이즈 없애기)

라인트레이서 차체에는 카메라를 답니다. 이 카메라는 가로 128픽셀에 광량을 0~255까지 받습니다. 어두운 부분은 0, 가장 밝은 부분은 255 값입니다. 이 카메라를 효과적으로 사용하려면 필터코드를 짜야 합니다. 

![SE-81d41897-88ae-426b-8f6d-ebef01761c6b](https://user-images.githubusercontent.com/52185595/114294981-330ef400-9add-11eb-9dc0-9f9bdd82eba8.png)

필터코드는 다음 파일 내부에 들어있습니다. 

위 동영상을 보면, 버튼을 누를 때마다 다른 필터가 적용됩니다. main.c 내에 필터 스위칭 부분을 살펴봅시다. 


```C++
if(BUTTON2_PRESS)
      {   
         Delay_ms(100);
         if(BUTTON2_PRESS)   mode = (mode+1)%4;
      }
      
      if(BUTTON1_PRESS)
      {
         Delay_ms(100);
         if(BUTTON1_PRESS)   mode = (mode+4-1)%4;
      }
      OLED_clearDisplay();
      
      switch(mode)
      {
         //Origin value
         case 1:
            for(i=0; i<128; i++)
            {
               OLED_drawYLine(i, 64-(CAM_DATA_PRE_1[i]/32),(CAM_DATA_PRE_1[i]/32));
               if(max<CAM_BUFFER[i])
                  max = CAM_BUFFER[i];
               if(min>CAM_BUFFER[i])
                  min = CAM_BUFFER[i];      
            }
         break;
         //mean filter
         case 2:
            for(i=0; i<128; i++)
            {
               OLED_drawYLine(i, 64-(CAM_DATA_MEAN[i]/32),(CAM_DATA_MEAN[i]/32));
               if(max<CAM_DATA_MEAN[i])
                  max = CAM_DATA_MEAN[i];
               if(min>CAM_DATA_MEAN[i])
                  min = CAM_DATA_MEAN[i]; 
            }
         break;
         //Rectifier
         case 3:
            for(i=0; i<128; i++)
            {   
               OLED_drawYLine(i, 64-(CAM_DATA_RECT[i]/32),(CAM_DATA_RECT[i]/32));
               if(max<CAM_DATA_RECT[i])
                  max = CAM_DATA_RECT[i];
               if(min>CAM_DATA_RECT[i])
                  min = CAM_DATA_RECT[i];
            }
         break;
         //Normalize
         case 0:
            for(i=0; i<128; i++)
            {   

               OLED_drawYLine(i, 64-(CAM_DATA_NORMALIZED[i]/32),(CAM_DATA_NORMALIZED[i]/32));
               if(max<CAM_DATA_NORMALIZED[i])
                  max = CAM_DATA_NORMALIZED[i];
               if(min>CAM_DATA_NORMALIZED[i])
                  min = CAM_DATA_NORMALIZED[i]; 
            }
         break;
         default:
         break;
      }
```
원래 카메라에서 받아온 데이터값은 CAM_DATA _BUFFER 입니다. 

​

스위치 모드 1일 때: CAM_DATA_MEAN ( 민 mean 필터) 

스위치 모드 2일 때: CAM_DATA_RECT (렉티파이 rectify 필터) 

스위치 모드 3일 때: CAM_PRE   CAM_DATA_NORMALIZED (노멀라이즈 필터)

스위치 모드 0일 때: CAM_PRE 

> 버퍼값(처음에 읽어오는 값) -> 프리 필터 -> 민 필터 -> 렉티파이 필터 -> 노멀라이즈 필터

순으로 데이터를 처리합니다. 

각각의 필터를 거친 후 데이터를 사진으로 보면:

1.  프리 필터 

![SE-0e0043eb-aea6-49ce-8600-18c043242442](https://user-images.githubusercontent.com/52185595/114295065-d6f89f80-9add-11eb-9f16-d06e123e1c8f.png)

2. 민 필터

![SE-247819ff-2c25-464a-89ab-255e56748221](https://user-images.githubusercontent.com/52185595/114295075-eb3c9c80-9add-11eb-9db7-313115f518f4.png)

3. 렉티파이 필터 

![SE-ccfc1961-045b-4a27-958e-5b216e623853](https://user-images.githubusercontent.com/52185595/114295094-014a5d00-9ade-11eb-91f2-9aaed2ec674b.png)

4. 노멀라이즈 필터

![SE-e7c00252-fc56-4263-9773-ab5ffa9b175a](https://user-images.githubusercontent.com/52185595/114295102-07d8d480-9ade-11eb-9278-7545ccb51bd8.png)

그럼 각각의 필터가 어떻게 작동하는지 살펴보도록 하겠습니다. 카메라 필터는 결국 '어떻게 내가 원하는 값만 잘 사용할 것인가'가 관건입니다. 그러기 위해서는 노이즈를 잘 처리해야 합니다. 

### 1. 프리 필터

값 중에 최솟값을 전체에서 빼 줍니다. 그래프를 전체적으로 아래로 평행이동 시킵니다. 

![스크린샷_2020-01-10_오전_12 13 11](https://user-images.githubusercontent.com/52185595/114295127-2dfe7480-9ade-11eb-92d6-1b19a7a15258.png)

### 2. 민 필터 

평균을 내 주는 필터입니다. 왼쪽으로 n개, 오른쪽으로 n개 값과 본인 값을 모두 더해서 2n+1으로 나눠줍니다. 

![스크린샷_2020-01-10_오전_12 17 45](https://user-images.githubusercontent.com/52185595/114295138-3a82cd00-9ade-11eb-82c3-ab916f09a78e.png)

그러면 노이즈 낀 부분이 위로 올라오면서 노이즈가 없어졌습니다. 하지만 정확도는 좀 떨어집니다. 

이 프로젝트에서는 민 필터는 렉티파이 필터를 위한 값을 만드는 데 사용됩니다. 

### 3. rectify 필



## keil uvision, stlink 설치하기 + 코드 업로드하기

keil은 코드 에디터입니다. 화면 구성은 이렇습니다. 화살표로 가리킨 아이콘을 누르면 코드를 빌드할 수 있습니다. 
참고로 맥용은 없습니다. 리눅스에서 사용하는 것도 까다로우므로, 웬만하면 윈도우 환경에서 사용하는 것이 마음 편합니다. 
파일이 커서 서피스 고 같은 곳에서 잘 안 돌아갑니다. 

![SE-4edeecdd-aecb-496d-b172-0ddc57ccc38d](https://user-images.githubusercontent.com/52185595/114294870-5e451380-9adc-11eb-9a46-135de2a5e612.png)
![66](https://user-images.githubusercontent.com/52185595/114294871-600ed700-9adc-11eb-82f0-d0b0f52394e0.png)

이렇게 keil 에서 빌드를 하고 나면, stlink 를 통해 마이크로컨트롤러에 코드를 업로드할 수 있습니다. 아래 usb는 마이크로컨트롤러와 컴퓨터를 연결시켜줍니다. 원하는대로 케이블을 저 usb에 꽂으면됩니다. GND와 3.3V를 제대로 꽂아 주어야 에러가 안 납니다. 

![SE-8cfa29ad-2369-4846-bba2-0a1079db6cfc](https://user-images.githubusercontent.com/52185595/114294884-79178800-9adc-11eb-82a6-8f9b9678571f.jpeg)

우선 keil에서 코드를 빌드시키고, stlink 프로그램을 실행시킵니다. 

![SE-3bbcba57-6d0f-41b0-ae53-7782da9387a0](https://user-images.githubusercontent.com/52185595/114294887-7ae14b80-9adc-11eb-8c44-f95ba257e874.png)

그러면 이런 창이 뜹니다. 여기서 view 아래에 있는 콘센트같은 아이콘을 클릭하면 마이크로컨트롤러와 컴퓨터를 연결할 수 있습니다.

![SE-1e32653f-8510-46a6-b926-72a39e771a4f](https://user-images.githubusercontent.com/52185595/114294907-a8c69000-9adc-11eb-8798-2ca776147b38.png)

연결이 제대로 된다면 이런 창이 뜹니다. 이후, program verify라는 아이콘을 클릭합니다. 

![44](https://user-images.githubusercontent.com/52185595/114294915-abc18080-9adc-11eb-8800-97cd166f7660.png)

그리고 keil에서 빌드한 코드를 엽니다. 

![55](https://user-images.githubusercontent.com/52185595/114294932-c693f500-9adc-11eb-9de8-e22cac1660a5.png)

그러면 이런 창이 뜨고, 여기서 start를 눌러주면 끝!
