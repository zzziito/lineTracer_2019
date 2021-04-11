# 전자공학과 돌쇠 2학기 세미나

## 목차
1. 납땜하기
2. 카메라 필터에 관해 (노이즈 없애기)
3. 주행코드 설명 (line by line)
4. keil uvision, stlink 설치하기 + 코드 업로드하기
5. Sketchup 3d 프린터로 카메라 거치대 만들기 

## 임베디드 프로그래밍을 이용한 라인트레이서 만들기 

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
원래 카메라에서 받아온 데이터값은 CAM_DATA_BUFFER 입니다. 


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

### 3. rectify 필터

프리 데이터와 민 데이터를 비교해보면, 

![SE-6c1fd0f9-5bc6-4e0e-91fe-f3faafbb523a](https://user-images.githubusercontent.com/52185595/114295171-71f17980-9ade-11eb-8fc1-29c3bd793b8b.png)

이런 식으로 표현할 수 있습니다. 민 필터는 평균을 냈기 때문에 좀 더 완만합니다. 렉티파이 필터는 PRE > MEAN 이면 그냥 값을 최댓값인 1024로 만듭니다. PRE < MEAN 이면

PRE[x]× 1024 / MEAN[x]

라는 수식에 값을 집어 넣습니다. 

![SE-933c7b03-270c-4d00-9987-844811e4975a](https://user-images.githubusercontent.com/52185595/114295198-95b4bf80-9ade-11eb-91f1-6464db7c3ee6.png)

그러면 아래와 같은 데이터를 얻을 수 있습니다. 즉, rectify 필터는 경계에서 값이 확 줄어듦으로써 흰-검 경계의 위치를 알려줍니다. 

### 4. normalize 필터

일반적인 노멀라이즈 필터는 다음과 같이 작동합니다. 

![SE-36cf3cd6-f143-47ee-be44-f7d505952b80](https://user-images.githubusercontent.com/52185595/114295214-b11fca80-9ade-11eb-8de4-0944336d3b5f.png)

일단 흰색, 검은색을 카메라로 봐서 white reference와 black reference 를 저장합니다. 그리고 버퍼값 각 픽셀에 다음과 같은 식을 적용합니다. 

![스크린샷 2021-04-11 오후 3 58 33](https://user-images.githubusercontent.com/52185595/114295219-c8f74e80-9ade-11eb-90d8-9e26a145c0e6.png)

![SE-be4ec398-a748-4304-b7d7-45824bf0c5d1](https://user-images.githubusercontent.com/52185595/114295230-d44a7a00-9ade-11eb-957d-b496121326f3.png)

이 식을 적용하면 다음과 같이 그래프가 쭉 펴집니다. 

![SE-e47de2bc-c119-490c-aabe-688e503260c3](https://user-images.githubusercontent.com/52185595/114295241-eb896780-9ade-11eb-821c-2c5fa41ceb82.png)

그리고 이런 식으로 우리가 원하는 데이터가 작게 나타날 때, 노멀라이즈 필터를 적용하면 그 데이터 변동을 크게 키울 수 있습니다. 

그래서 rectify 데이터가 normalize 필터를 거치면 값이 훨씬 많이 떨어지고, 노이즈 제거도 되는 것을 볼 수 있습니다. 


우리는 이후 라인트레이서 코드를 짤 때 이 normalize 데이터 , 즉 CAM_DATA_NORMALIZED를 사용할 것입니다. 

## 주행코드


라인트레이서 최종코드를 첨부합니다. 이를 main.c에 붙여넣으면 작동합니다.

이 부분은 버튼 누르면 카메라 필터 바뀌어서 디스플레이 되는 코드입니다. 

```c++
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
         case 3:
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
         case 1:
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
         case 2:
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


사실 달리기만 하는 코드는 짱 쉽습니다. 


```c++
if(CAM_DATA_NORMALIZED[63] < 1024)
   {
      Motor_Go(500,500);
            //motor running
      if((CAM_DATA_NORMALIZED[48]+CAM_DATA_NORMALIZED[78])/2 < 450)
      {
         while(1)
         {
            Motor_Go(0,0);
         }
      }
   }
   
   else if(CAM_DATA_NORMALIZED[63] == 1024)
   {
      int turningpoint = 0;
      for(i=0;i<63;i++)
      {
         turningpoint = 950;
         if((turningpoint+CAM_DATA_NORMALIZED[i])/2 < 984)
         {
            Motor_Go(300,600);
         }
         //left cornering
         
         else if((turningpoint+CAM_DATA_NORMALIZED[127-i])/2 < 984)
         {
            Motor_Go(600,300);
         }
         //right cornering
      }   
```
저희 동아리 어떤 조는 이렇게 짜기도 했습니다. 제 코드가 150줄을 넘어가는 것에 비해 단순합니다. 가긴 가는데 정확도가 떨어지고 움직임이 부드럽지 않습니다. 

대회에서는 

![SE-2a2c985c-845e-4811-809c-5e9ae576dd76](https://user-images.githubusercontent.com/52185595/114295311-79fde900-9adf-11eb-8abd-4eeef15a0823.png)

이 스쿨존을 인식하고 이 구간에서는 속도를 늦춰야 합니다. 

![스크린샷_2020-01-19_오후_9 35 39](https://user-images.githubusercontent.com/52185595/114295316-7f5b3380-9adf-11eb-86f7-8d729035dda5.png)

그리고 이 마크를 만나면 정지해야합니다. 

```c++
 a=0;
      count=0;
      max = 0;
      min = 0xffff;
      schoolZoneCounter=0;
      q=0;
      k=0;
       
        for(i=0; i<127; i++){
                     stop[i]=0;
                     filter[i]=0;
         }
```

이 부분은 그냥 초기에 변수 다 초기화하는 거임 int main(void)  앞부분에 변수 초기화할건지 while (1) 내에서 변수 초기화할건지 잘 구분해야 합니다. 

```c++
for (i=0; i<128; i++){
      if(CAM_DATA_NORMALIZED[i]<700 && (CAM_DATA_MEAN[i]-CAM_DATA_PRE_1[i]<1000)){
                q=q+1;
        if(q>2){
            filter[count] = CAM_DATA_MEAN[i] - CAM_DATA_PRE_1[i];
            a=a+i;
            count+=1;
            schoolZone[i]=1;
         }
      }
        else{
            schoolZone[i]=0;
                 q=0;
         }    
      }
```

이전에 CAM_DATA_NORMALIZED 라는 데이터를 쓸 거고, 이 데이타는 경계값에 대한 정보를 준다고 했습니다. 이 부분은 노이즈 제거를 위한 코드입니다. 

까만 선을 따라가도록 코드를 짰는데, 티끌을 까만거로 인식하거나 그래서 자꾸 트랙을 이탈하는 일이 생기는 문제가 있었습니다. 

그래서 노이즈 제거를 위한 변수를 여러가지 썼습니다. 

1. q 이용 

2. a 이용 

(나혼자 볼 거라서 변수 이름을 대충 지었는데 후회막심쓰...)


q는 이런 식입니다.
: 만약에 노이즈라면 하락하는 값이 몇 픽셀 안될 거고 유의미한 값이라면 그래도 트랙이 좀 굵으니까 많은 픽셀의 값이 낮아질 것입니다. 그래서  만약에 픽셀값이 특정 범위 내에 있다면 q를 1씩 증가시키고, q가 특정 갯수 이상일 때만 라인 따라가기 코드가 작동되도록 했습니다. 

![스크린샷_2020-01-19_오후_9 45 51](https://user-images.githubusercontent.com/52185595/114295387-ebd63280-9adf-11eb-98ca-039c66da36bb.png)

그리고 라인 따라가기 코드가 작동하면 (q>2이면) a라는 변수는 해당 픽셀값을 모두 더하고, count 는 해당 픽셀의 개수를 모두 더합니다. 
그리고 direction = a/count 를 하면, 트랙의 가운데가 있는 픽셀의 위치를 알 수있음. 트랙의 가운데가 direction이라는 뜻이고, 
이 direction을 

```
speed_right = 650 + 5*(64 - direction);
speed_left = 650 - 3*(64 - direction);
```

이런 식으로 좌우 모터에 넣으면 달리는 건 할 수 있었습니다. 

이 정도 코드면 이만큼 움직일 수 있습니다. 

좀 더 다듬으면 이만큼 움직일 수 있습니다. 

### 스쿨존 미션 해결

이전에 우리는 schoolZone 이라는 배열에 1을 집어넣었습니다. 그러므로 schoolZone 배열에는 이런 식으로 값이 들어갈 것입니다. 

![스크린샷_2020-01-19_오후_10 11 30](https://user-images.githubusercontent.com/52185595/114295485-79198700-9ae0-11eb-8de7-d59ded5359f6.png)

그러면 schoolZone 배열에서 0-1 이 바뀌는 횟수를 세서 검은 줄이 한 개인지 세 개인지 알 수 있을 것입니다. 

```
for (i=0; i<127; i++){
      
         if(schoolZone[i]!=schoolZone[i+1]){
            
               stop[schoolZoneCounter]=i;
                
               schoolZoneCounter+=1;
         }    
      }
```

schoolZone[i]!=schoolZone[i+1] 이면 schoolZoneCounter 값이 1씩 늘어나도록 해 놓았습니다. 

그런데, 

![스크린샷_2020-01-19_오후_10 22 26](https://user-images.githubusercontent.com/52185595/114295494-99e1dc80-9ae0-11eb-90f0-67fa257260e1.png)

내가 지금 마크를 보고 있다- 는 건 인지할 수 있는데,  마크 본 이후부터 다시 마크를 볼 때까지가 스쿨존이라는 걸 인식하는건 무척 어려웠습니다. 그래서 다음과 같은 방법으로 문제를 해결했습니다. 

```c++
if ((schoolZoneCounter>=6 && schoolZoneCounter<13) && b==0){ 
          b=1;
      }
      
       else if(schoolZoneCounter<6 && b==1){   
         if(schoolZoneBulean == 0){
            schoolZoneBulean =1;
            b=0;
         }
         else {
            schoolZoneBulean=0;
            b=0;
         }   
      }
```
![스크린샷_2020-01-19_오후_10 34 17](https://user-images.githubusercontent.com/52185595/114295519-c39b0380-9ae0-11eb-9e51-6a29d109b137.png)

다음 포문을 거치는 동안 schoolZoneBulean은 파란색같이 변합니다. 이제 우리는 스쿨존을 인식할 수 있습니다!

이제 남은 것은 스쿨존 내에서 모터에 들어가는 속도를 줄이기만 하면 됩니다. 

```c++
 for (i=0; i<127; i++){
      
         if(schoolZone[i]!=schoolZone[i+1]){
            
               stop[schoolZoneCounter]=i;
                
               schoolZoneCounter+=1;
         }    
      }

     k=stop[3]-stop[0]; 

 if(k>60 && rightFlag==0 && leftFlag==0 && schoolZoneCounter>2 && schoolZoneCounter<6){
        
        stopbool=1;
        
      }
```

정지는 이렇게 구현했습니다. stop 사인은 가로로 기니까, 가로 길이가 일정 이상 넘어가는 까만 부분이 있으면 stopbool 이 1 이 되도록 했습니다. 

### flag

flag 는 


자꾸 이렇게 트랙을 벗어나길래, 뭔가 카메라 인식에 문제가 생겨도 돌던 방향으로 계속 돌 수 있도록 하는 것입니다. 



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

## Sketchup 3d 프린터로 카메라 거치대 만들기 


