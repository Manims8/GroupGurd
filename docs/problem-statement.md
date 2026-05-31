# Problem Statement

Group travel in crowded and dynamic environments creates a real risk of accidental separation. This is especially stressful when a group includes children, elderly people, or members who may not be able to communicate quickly in noisy public spaces.

Traditional solutions such as public announcements, CCTV monitoring, police assistance, or calling by phone are mostly reactive. They help after a person is already missing, which can cause panic, delay, and emotional distress.

GroupGuard addresses this by providing a proactive wearable system that continuously monitors group proximity and alerts members before separation becomes serious.

## Target Scenario

The system is intended for:

- Temples and pilgrimage areas
- Festivals and fairs
- Tourist destinations
- Crowded markets
- College or school trips
- Family group travel

## Objectives

- Monitor relative distance between group members in real time
- Provide vibration and LED alerts when a member moves beyond a safe threshold
- Show directional guidance on leader/display bands
- Allow any member to send an SOS alert
- Work without internet, phone apps, or external infrastructure
- Stay low-cost and simple enough for real-world wearable use

## Design Direction

The project uses ESP32 boards because they provide WiFi, TCP networking, GPIO control, and enough processing power at low cost. RSSI is used for approximate proximity detection, while QMC5883L compass readings provide direction cues. This combination is not meant for exact indoor positioning; it is meant to answer the practical safety question: "Who is too far, and which way should I look?"
