

# 💻 Algorithm Battle Linux Project

- 알고리즘을 누가 더 빨리 푸는 지 겨룰 수 있는 c로 작성된 프로그램

## 📖 프로젝트 개발 배경
- 학생들은 대부분 알고리즘을 공부할 대 백준 등의 채점 사이트를 이용한다.
- 채점 사이트에서 알고리즘을 공부할 때 학생들은 문제를 풀고 채점 받고 다른 문제를 또 풀고 채점받는 단순한 과정을 반복하게 된다.
- 이에 단순 반복 속에서 알고리즘 공부에 대한 지루함을 느끼는 학생들이 많은데 우리 팀은 이러한 점을 문제점으로 정의하고 솔루션으로써 알고리즘 배틀을 말한다.
- 알고리즘 배틀은 기존의 단순한 알고리즘 공부 방법에서 벗어난 놀이적인 알고리즘 공부 방법 제공과, 실력을 겨룰 수 있는 장을 제공하는 것을 목표로 한다.
## 🚂 프로젝트 주요 내용
- 알고리즘 겨루기 프로그램은 랜덤으로 출제된 알고리즘 문제를 누가 더 빨리 푸는 지 2명의 클라이언트가 겨룰 수 있도록 하는 프로그램이다.
- 알고리즘 겨루기 프로그램은 1:1겨루기만을 지원하며, 서버 하나에 클라이언트 둘이 매달리는 구조를 가진다.
- 클라이언트는 서버의 쓰레드에 의해 핸들링 되며, 클라이언트와 서버는 소켓 통신을 기반으로 동작한다.

## ⤵️ 프로젝트 흐름
- ![image](https://github.com/Libienz/AlgorithmBattle-Linux/assets/85234650/336071a0-73f0-4ec8-b02f-4ab230717ae6)
- 0 소켓연결: 서버와 두 클라이언트가 소켓으로 연결 된다.
- 1 문제 출제: 서버가 자신이 가지고 있는 문제들 중 랜덤으로 문제를 골라 client들에게 출제한다.
- 2 문제 풀이: 클라이언트들은 출제된 문제를 c소스로 편집하며 푼다.
- 3 소스파일 제출: 클라이언트가 자신이 정답이라고 생각하는 소스파일을 서버에 제출한다.
- 4 채점: 서버는 제출 받은 제출물을 채점 프로세스로 채점해본다.
- 5 대결 종료: 정답 여부를 확인하고 먼저 풀어낸 승자가 나왔다면 이를 클라이언트들에게 알리고 종료한다.

## 🛠️ 시스템 설계도
### 서버 설계도
 - ![image](https://github.com/Libienz/AlgorithmBattle-Linux/assets/85234650/12483cd0-9cc5-4e49-9a53-2eaa6beb5069)
- 서버는 문제 목록과 답지 목록을 가지고 있으며 채점하는 심판인 judge를 구성요소로써 가진다.
- 서버와 클라이언트의 연결은 소켓을 매개로 되어 있으며 서버는 연결된 두명의 클라이언트를 각각 쓰레드로 핸들링한다.
- 여러 스레드가 critical section에 접근하는 것을 막기 위하여 mutex lock을 이용하였고 mutual exclusion을 적절히 보장하도록 설계되어 있다.


### 채점 동작 

- ![image](https://github.com/Libienz/AlgorithmBattle-Linux/assets/85234650/1a0f7442-ab7a-4813-aa4b-d2160c2340a9)
- 서버의 채점기는 서버 프로그램에서 내부적으로 사용하는 모듈화 된 프로그램이며 서버는 제출물을 받을 때 마다 fork를 하여 채점기를 생성해낸다.
- 호출된 재점기는 다시 fork를 하여 유저 제출물의 실행 파일을 실행시키며 오답과 정답을 검증한다.
- 자식와 부모 프로세스 사이의 IPC는 pipe 구조를 이용했으며 양방향 통신을 위해 2개를 설계하였다.

### 클라이언트 설계도
- ![image](https://github.com/Libienz/AlgorithmBattle-Linux/assets/85234650/dea3db77-6ccc-475b-9b2d-b84d03d34c01)
- 클라이언트는 두개의 쓰레드를 가진다.
- 하나는 서버로 부터 오는 메시지를 수신하는 수신 쓰레드이며 다른 하나는 제출물을 전송하기 위한 전송 쓰레드이다.
- 수신 쓰레드와 전송 쓰레드 모두 소켓 프로그래밍을 기반으로 동작한다.



