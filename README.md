# doctor_oop

C++ система учета пациентов и болезней с frontend на `HTML/CSS/JS`.

Что реализовано:
- `PostgreSQL` база данных;
- регистрация и логин для двух ролей: `doctor` и `patient`;
- кабинет врача: список пациентов с болезнями;
- кабинет врача: футуристичные панели добавления/редактирования болезней, toggles и фильтры;
- кабинет пациента: персональная карта болезней (с анимациями/8-bit режимом);
- переключение языка интерфейса `RU/EN`;
- legacy endpoint поиска болезней по фамилии (`/api/diseases?surname=...`).

## Архитектура
- `src/` — backend на C++ (сокетный HTTP API + слой БД через `libpq`).
- `frontend/` — киберпанк UI (фиолетовые оттенки, анимации, role-based экраны).
- `docker-compose.yml` — orchestration для `postgres + backend + frontend`.

## Быстрый старт (Docker)

```bash
docker compose up --build
```

После запуска:
- Frontend: `http://localhost:3000`
- Backend API: `http://localhost:8080`

Остановка:
```bash
docker compose down
```

Сброс БД (если нужен чистый старт):
```bash
docker compose down -v
```

## Demo-учетки

- Врач: `doctor@clinic.local / doctor123`
- Пациент: `ivanov@clinic.local / patient123`

## Основные API маршруты

- `GET /api/health`
- `GET /api/doctors`
- `POST /api/register/doctor`
- `POST /api/register/patient`
- `POST /api/login`
- `GET /api/doctor/patients` (Bearer token врача)
- `GET /api/doctor/diseases/library` (Bearer token врача)
- `POST /api/doctor/patient/add-disease` (Bearer token врача)
- `POST /api/doctor/patient/update-disease` (Bearer token врача)
- `GET /api/patient/diseases` (Bearer token пациента)
- `GET /api/diseases?surname=...` (legacy совместимость)

Формат `POST`: `application/x-www-form-urlencoded`.

## Примеры запросов

Логин врача:
```bash
curl -X POST http://localhost:8080/api/login \
  -H "Content-Type: application/x-www-form-urlencoded" \
  --data "role=doctor&email=doctor@clinic.local&password=doctor123"
```

Получить пациентов врача:
```bash
curl http://localhost:8080/api/doctor/patients \
  -H "Authorization: Bearer <TOKEN>"
```

## Локальная сборка backend (без Docker)

Нужен `libpq-dev` (или equivalent PostgreSQL client headers/libs).

```bash
clang++ -std=c++17 src/*.cpp -lpq -o doctor_oop
```

Переменная подключения к БД:
- `DATABASE_URL` (например: `postgresql://doctor_app:doctor_app@localhost:5432/doctor_oop`)
