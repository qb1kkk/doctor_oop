const API_BASE = window.location.port === "3000"
    ? "/api"
    : `http://${window.location.hostname || "localhost"}:8080/api`;

const translations = {
    ru: {
        logout: "Выйти",
        title: "Реестр болезней пациента",
        subtitle: "Регистрация и кабинеты врача/пациента с клиническими данными",
        loginTab: "Вход",
        registerTab: "Регистрация",
        roleLabel: "Роль",
        roleDoctor: "Врач",
        rolePatient: "Пациент",
        emailLabel: "Email",
        passwordLabel: "Пароль",
        loginBtn: "Войти",
        doctorNameLabel: "ФИО врача",
        patientFirstNameLabel: "Имя пациента",
        patientLastNameLabel: "Фамилия пациента",
        doctorSelectLabel: "Лечащий врач",
        registerDoctorBtn: "Зарегистрировать врача",
        registerPatientBtn: "Зарегистрировать пациента",
        demoHint: "Демо-аккаунты: doctor@clinic.local / doctor123, ivanov@clinic.local / patient123",
        doctorCabinetTitle: "Кабинет врача",
        doctorCabinetHint: "Список ваших пациентов и их болезней:",
        patientCabinetTitle: "Кабинет пациента",
        waitingData: "Ожидание данных...",
        welcomeDoctor: "Вы вошли как врач",
        welcomePatient: "Вы вошли как пациент",
        authRequired: "Требуется вход в систему.",
        serverError: "Ошибка сервера. Попробуйте позже.",
        loginError: "Неверный логин или пароль.",
        registerDoctorOk: "Врач зарегистрирован. Теперь выполните вход.",
        registerPatientOk: "Пациент зарегистрирован. Теперь выполните вход.",
        doctorsLoadError: "Не удалось загрузить список врачей.",
        noDoctors: "Сначала зарегистрируйте врача.",
        noPatients: "У врача пока нет пациентов.",
        noDiseases: "Болезни не назначены.",
        noChronicDiseases: "По фильтру хронических болезней данных нет.",
        doctorPatientsLoadError: "Не удалось загрузить пациентов врача.",
        patientDataLoadError: "Не удалось загрузить данные пациента.",
        patientCardTitle: "Карта заболеваний",
        doctorLabel: "Лечащий врач",
        icdLabel: "МКБ-10",
        chronic: "хроника",
        acute: "острое",
        hoverHint: "Наведи курсор для раскрытия мед-панели",
        treatmentTitle: "Курс лечения",
        transformText: "Трансформация экрана в 8-bit режим...",
        unknownDisease: "Подробная карточка болезни отсутствует в базе.",
        genericStep1: "Повторная консультация врача.",
        genericStep2: "Уточняющая диагностика по профилю.",
        genericStep3: "Индивидуальный план терапии.",
        refreshBtn: "Обновить",
        chronicOnlyToggle: "Только хронические",
        addDiseasePanelTitle: "Назначить болезнь",
        editDiseasePanelTitle: "Редактировать болезнь",
        existingMode: "Из библиотеки",
        customMode: "Новая болезнь",
        diseaseLibraryLabel: "Библиотека болезней",
        diseaseNameLabel: "Название болезни",
        icdCodeLabel: "Код МКБ-10",
        chronicToggle: "Хроническая болезнь",
        descriptionLabel: "Описание",
        treatmentLabel: "Курс лечения (через ;)",
        assignDiseaseBtn: "Назначить болезнь",
        saveDiseaseBtn: "Сохранить изменения",
        addDiseaseAction: "Добавить болезнь",
        editDiseaseAction: "Редактировать",
        chooseDisease: "Выберите болезнь",
        diseaseLibraryLoadError: "Не удалось загрузить библиотеку болезней.",
        addDiseaseSuccess: "Болезнь назначена пациенту.",
        addDiseaseError: "Не удалось назначить болезнь.",
        updateDiseaseSuccess: "Карточка болезни обновлена.",
        updateDiseaseError: "Не удалось обновить болезнь."
    },
    en: {
        logout: "Logout",
        title: "Patient Disease Registry",
        subtitle: "Doctor/patient registration and role-based clinical dashboards",
        loginTab: "Login",
        registerTab: "Register",
        roleLabel: "Role",
        roleDoctor: "Doctor",
        rolePatient: "Patient",
        emailLabel: "Email",
        passwordLabel: "Password",
        loginBtn: "Sign in",
        doctorNameLabel: "Doctor full name",
        patientFirstNameLabel: "Patient first name",
        patientLastNameLabel: "Patient last name",
        doctorSelectLabel: "Attending doctor",
        registerDoctorBtn: "Register doctor",
        registerPatientBtn: "Register patient",
        demoHint: "Demo accounts: doctor@clinic.local / doctor123, ivanov@clinic.local / patient123",
        doctorCabinetTitle: "Doctor Dashboard",
        doctorCabinetHint: "Your patients and their diseases:",
        patientCabinetTitle: "Patient Dashboard",
        waitingData: "Waiting for data...",
        welcomeDoctor: "Signed in as doctor",
        welcomePatient: "Signed in as patient",
        authRequired: "Authentication required.",
        serverError: "Server error. Please try again later.",
        loginError: "Invalid email or password.",
        registerDoctorOk: "Doctor registered. You can sign in now.",
        registerPatientOk: "Patient registered. You can sign in now.",
        doctorsLoadError: "Failed to load doctors list.",
        noDoctors: "Register at least one doctor first.",
        noPatients: "No patients assigned yet.",
        noDiseases: "No diseases assigned.",
        noChronicDiseases: "No chronic diseases by current filter.",
        doctorPatientsLoadError: "Failed to load doctor patients.",
        patientDataLoadError: "Failed to load patient data.",
        patientCardTitle: "Disease Card",
        doctorLabel: "Doctor",
        icdLabel: "ICD-10",
        chronic: "chronic",
        acute: "acute",
        hoverHint: "Hover to open medical panel",
        treatmentTitle: "Treatment course",
        transformText: "Transforming to 8-bit display...",
        unknownDisease: "No detailed disease card in DB.",
        genericStep1: "Repeat consultation with doctor.",
        genericStep2: "Additional diagnostics by profile.",
        genericStep3: "Personalized therapy plan.",
        refreshBtn: "Refresh",
        chronicOnlyToggle: "Chronic only",
        addDiseasePanelTitle: "Assign disease",
        editDiseasePanelTitle: "Edit disease",
        existingMode: "From library",
        customMode: "New disease",
        diseaseLibraryLabel: "Disease library",
        diseaseNameLabel: "Disease name",
        icdCodeLabel: "ICD-10 code",
        chronicToggle: "Chronic disease",
        descriptionLabel: "Description",
        treatmentLabel: "Treatment plan (split by ;)",
        assignDiseaseBtn: "Assign disease",
        saveDiseaseBtn: "Save changes",
        addDiseaseAction: "Add disease",
        editDiseaseAction: "Edit",
        chooseDisease: "Select disease",
        diseaseLibraryLoadError: "Failed to load disease library.",
        addDiseaseSuccess: "Disease assigned to patient.",
        addDiseaseError: "Failed to assign disease.",
        updateDiseaseSuccess: "Disease card updated.",
        updateDiseaseError: "Failed to update disease."
    }
};

const diseaseFallback = {
    "Гипертония": {
        description: "Стойкое повышение артериального давления с риском сердечно-сосудистых осложнений.",
        treatment: [
            "Контроль давления ежедневно.",
            "Снижение соли и веса.",
            "Постоянная антигипертензивная терапия."
        ]
    },
    "Гастрит": {
        description: "Воспаление слизистой желудка с болевым синдромом.",
        treatment: [
            "Щадящая диета.",
            "ИПП и антациды по схеме.",
            "Контроль Helicobacter pylori."
        ]
    },
    "Бронхиальная астма": {
        description: "Хроническое воспаление дыхательных путей с бронхообструкцией.",
        treatment: [
            "Ингаляционные ГКС как базис.",
            "Бронхолитик для приступов.",
            "Контроль триггеров."
        ]
    },
    "ОРВИ": {
        description: "Острая респираторная вирусная инфекция.",
        treatment: [
            "Покой и питьевой режим.",
            "Симптоматическая терапия.",
            "Контроль состояния 3-5 дней."
        ]
    },
    "Сахарный диабет 2 типа": {
        description: "Метаболическое заболевание с хронической гипергликемией.",
        treatment: [
            "Диета и снижение массы тела.",
            "Контроль глюкозы и HbA1c.",
            "Сахароснижающая терапия."
        ]
    }
};

const ui = {
    mainPanel: document.getElementById("mainPanel"),
    authSection: document.getElementById("authSection"),
    dashboardSection: document.getElementById("dashboardSection"),
    doctorDashboard: document.getElementById("doctorDashboard"),
    patientDashboard: document.getElementById("patientDashboard"),
    doctorPatients: document.getElementById("doctorPatients"),
    result: document.getElementById("result"),
    authMessage: document.getElementById("authMessage"),
    welcomeText: document.getElementById("welcomeText"),
    logoutBtn: document.getElementById("logoutBtn"),

    langRu: document.getElementById("langRu"),
    langEn: document.getElementById("langEn"),

    loginModeBtn: document.getElementById("loginModeBtn"),
    registerModeBtn: document.getElementById("registerModeBtn"),
    registerBlock: document.getElementById("registerBlock"),

    registerDoctorTab: document.getElementById("registerDoctorTab"),
    registerPatientTab: document.getElementById("registerPatientTab"),
    registerDoctorForm: document.getElementById("registerDoctorForm"),
    registerPatientForm: document.getElementById("registerPatientForm"),

    loginForm: document.getElementById("loginForm"),
    registerDoctorFormEl: document.getElementById("registerDoctorForm"),
    registerPatientFormEl: document.getElementById("registerPatientForm"),
    doctorSelect: document.getElementById("patientDoctorId"),

    refreshDoctorBtn: document.getElementById("refreshDoctorBtn"),
    chronicOnlyToggle: document.getElementById("chronicOnlyToggle"),

    panelBackdrop: document.getElementById("panelBackdrop"),
    addDiseasePanel: document.getElementById("addDiseasePanel"),
    editDiseasePanel: document.getElementById("editDiseasePanel"),
    closeAddPanel: document.getElementById("closeAddPanel"),
    closeEditPanel: document.getElementById("closeEditPanel"),

    addDiseaseForm: document.getElementById("addDiseaseForm"),
    addPatientId: document.getElementById("addPatientId"),
    addPatientLabel: document.getElementById("addPatientLabel"),
    addDiseaseMessage: document.getElementById("addDiseaseMessage"),
    modeExistingBtn: document.getElementById("modeExistingBtn"),
    modeCustomBtn: document.getElementById("modeCustomBtn"),
    existingModeBlock: document.getElementById("existingModeBlock"),
    customModeBlock: document.getElementById("customModeBlock"),
    libraryDiseaseId: document.getElementById("libraryDiseaseId"),
    addDiseaseName: document.getElementById("addDiseaseName"),
    addDiseaseIcd: document.getElementById("addDiseaseIcd"),
    addDiseaseChronic: document.getElementById("addDiseaseChronic"),
    addDiseaseDescription: document.getElementById("addDiseaseDescription"),
    addDiseaseTreatment: document.getElementById("addDiseaseTreatment"),

    editDiseaseForm: document.getElementById("editDiseaseForm"),
    editPatientId: document.getElementById("editPatientId"),
    editDiseaseId: document.getElementById("editDiseaseId"),
    editDiseaseLabel: document.getElementById("editDiseaseLabel"),
    editDiseaseMessage: document.getElementById("editDiseaseMessage"),
    editDiseaseName: document.getElementById("editDiseaseName"),
    editDiseaseIcd: document.getElementById("editDiseaseIcd"),
    editDiseaseChronic: document.getElementById("editDiseaseChronic"),
    editDiseaseDescription: document.getElementById("editDiseaseDescription"),
    editDiseaseTreatment: document.getElementById("editDiseaseTreatment")
};

let language = localStorage.getItem("lang") || "ru";
let token = localStorage.getItem("authToken") || "";
let role = localStorage.getItem("authRole") || "";
let displayName = localStorage.getItem("displayName") || "";
let pixelModeActivated = false;
let renderTimer = null;
let addPanelMode = "existing";
let doctorPatientsState = [];
let diseasesLibraryState = [];
let patientState = null;

function t(key) {
    return translations[language][key] || key;
}

function escapeHtml(value) {
    return String(value ?? "")
        .replaceAll("&", "&amp;")
        .replaceAll("<", "&lt;")
        .replaceAll(">", "&gt;")
        .replaceAll('"', "&quot;")
        .replaceAll("'", "&#39;");
}

function setMessage(text, isError = false) {
    ui.authMessage.textContent = text;
    ui.authMessage.className = isError ? "status status--err" : "status status--ok";
}

function setInlineMessage(element, text, isError = false) {
    element.textContent = text;
    element.className = isError ? "status status--err" : "status status--ok";
}

function clearMessage() {
    ui.authMessage.textContent = "";
    ui.authMessage.className = "status";
}

function setLanguage(nextLanguage) {
    language = nextLanguage;
    localStorage.setItem("lang", language);
    document.documentElement.lang = language;

    ui.langRu.classList.toggle("is-active", language === "ru");
    ui.langEn.classList.toggle("is-active", language === "en");

    document.querySelectorAll("[data-i18n]").forEach((element) => {
        const key = element.getAttribute("data-i18n");
        element.textContent = t(key);
    });

    if (role) {
        ui.welcomeText.textContent = `${role === "doctor" ? t("welcomeDoctor") : t("welcomePatient")}: ${displayName}`;
    }

    if (role === "doctor" && !ui.dashboardSection.classList.contains("hidden")) {
        renderDoctorPatients(doctorPatientsState);
        renderDiseasesLibrary();
    }
    if (role === "patient" && !ui.dashboardSection.classList.contains("hidden") && patientState) {
        renderPatientDashboard(patientState, false);
    }
}

function triggerHangarDoorAnimation() {
    ui.mainPanel.classList.remove("panel--doors-open");
    void ui.mainPanel.offsetWidth;
    ui.mainPanel.classList.add("panel--doors-open");
    window.setTimeout(() => ui.mainPanel.classList.remove("panel--doors-open"), 1100);
}

function activatePixelMode() {
    if (pixelModeActivated) {
        return;
    }
    pixelModeActivated = true;
    window.setTimeout(() => document.body.classList.add("mode-pixel"), 280);
}

function disablePixelMode() {
    pixelModeActivated = false;
    document.body.classList.remove("mode-pixel");
}

async function api(path, { method = "GET", body = null, requireAuth = true } = {}) {
    const headers = {};
    let requestBody;

    if (body) {
        headers["Content-Type"] = "application/x-www-form-urlencoded";
        requestBody = new URLSearchParams(body).toString();
    }

    if (requireAuth && token) {
        headers.Authorization = `Bearer ${token}`;
    }

    const response = await fetch(`${API_BASE}${path}`, {
        method,
        headers,
        body: requestBody
    });

    const text = await response.text();
    let data = {};
    try {
        data = text ? JSON.parse(text) : {};
    } catch {
        data = {};
    }
    return { ok: response.ok, status: response.status, data };
}

function getTreatmentSteps(disease) {
    if (disease.treatment && disease.treatment.trim()) {
        return disease.treatment
            .split(/;|\n/)
            .map((item) => item.trim())
            .filter(Boolean);
    }

    const fallback = diseaseFallback[disease.name];
    if (fallback) {
        return fallback.treatment;
    }
    return [t("genericStep1"), t("genericStep2"), t("genericStep3")];
}

function getDiseaseDescription(disease) {
    if (disease.description && disease.description.trim()) {
        return disease.description;
    }
    const fallback = diseaseFallback[disease.name];
    if (fallback) {
        return fallback.description;
    }
    return t("unknownDisease");
}

function buildPatientDiseaseHtml(disease, index) {
    const treatmentHtml = getTreatmentSteps(disease)
        .map((step) => `<li>${escapeHtml(step)}</li>`)
        .join("");

    return `
        <li class="disease" tabindex="0" style="animation-delay:${index * 110}ms">
            <div class="disease__head">
                <p class="disease__title">${escapeHtml(disease.name)}</p>
                <span class="disease__severity">${disease.chronic ? t("chronic") : t("acute")}</span>
            </div>
            <p class="disease__meta">${t("icdLabel")}: ${escapeHtml(disease.icdCode)}</p>
            <div class="disease__prompt">${t("hoverHint")}</div>
            <div class="disease__details">
                <p class="disease__desc">${escapeHtml(getDiseaseDescription(disease))}</p>
                <p class="disease__course-title">${t("treatmentTitle")}</p>
                <ul class="treatment-list">${treatmentHtml}</ul>
            </div>
        </li>
    `;
}

function renderPatientDashboard(patient, withTransform = true) {
    patientState = patient;

    if (withTransform) {
        triggerHangarDoorAnimation();
        activatePixelMode();
    }

    if (renderTimer) {
        clearTimeout(renderTimer);
    }

    ui.result.className = "result result--loading";
    ui.result.innerHTML = `<p>${escapeHtml(t("transformText"))}</p>`;

    const diseaseCards = patient.diseases.length
        ? patient.diseases.map((disease, index) => buildPatientDiseaseHtml(disease, index)).join("")
        : `<li class="disease is-open"><div class="disease__head"><p class="disease__title">${escapeHtml(t("noDiseases"))}</p></div></li>`;

    const renderFn = () => {
        ui.result.className = "result";
        ui.result.innerHTML = `
            <h2>${escapeHtml(t("patientCardTitle"))}: ${escapeHtml(patient.firstName)} ${escapeHtml(patient.lastName)}</h2>
            <p class="status">${escapeHtml(t("doctorLabel"))}: ${escapeHtml(patient.doctor)}</p>
            <ul class="disease-list">${diseaseCards}</ul>
        `;
    };

    if (withTransform) {
        renderTimer = window.setTimeout(renderFn, 560);
    } else {
        renderFn();
    }
}

function renderDoctorPatients(patients) {
    if (!patients.length) {
        ui.doctorPatients.innerHTML = `<p class="status">${escapeHtml(t("noPatients"))}</p>`;
        return;
    }

    const chronicOnly = ui.chronicOnlyToggle.checked;
    ui.doctorPatients.innerHTML = patients
        .map((patient, index) => {
            const filteredDiseases = chronicOnly ? patient.diseases.filter((disease) => disease.chronic) : patient.diseases;
            const diseasesHtml = filteredDiseases.length
                ? filteredDiseases.map((disease) => `
                    <li class="doctor-disease">
                        <span class="doctor-disease__meta">${escapeHtml(disease.name)} (${escapeHtml(disease.icdCode)})</span>
                        <button type="button"
                                class="micro-btn micro-btn--ghost edit-disease-btn"
                                data-patient-id="${patient.id}"
                                data-disease-id="${disease.id}">
                            ${escapeHtml(t("editDiseaseAction"))}
                        </button>
                    </li>
                `).join("")
                : `<li class="doctor-disease"><span class="doctor-disease__meta">${escapeHtml(chronicOnly ? t("noChronicDiseases") : t("noDiseases"))}</span></li>`;

            return `
                <article class="doctor-card" style="animation-delay:${index * 90}ms">
                    <p class="doctor-card__name">${escapeHtml(patient.firstName)} ${escapeHtml(patient.lastName)}</p>
                    <p class="doctor-card__meta">${escapeHtml(patient.email)}</p>
                    <ul class="doctor-card__diseases">${diseasesHtml}</ul>
                    <div class="doctor-card__footer">
                        <button type="button" class="micro-btn add-disease-btn" data-patient-id="${patient.id}">
                            ${escapeHtml(t("addDiseaseAction"))}
                        </button>
                    </div>
                </article>
            `;
        })
        .join("");
}

function renderDiseasesLibrary() {
    if (!diseasesLibraryState.length) {
        ui.libraryDiseaseId.innerHTML = `<option value="">${escapeHtml(t("chooseDisease"))}</option>`;
        return;
    }

    ui.libraryDiseaseId.innerHTML = diseasesLibraryState
        .map((disease) => `<option value="${disease.id}">${escapeHtml(disease.name)} (${escapeHtml(disease.icdCode)})</option>`)
        .join("");
}

function showLoginMode() {
    ui.loginModeBtn.classList.add("is-active");
    ui.registerModeBtn.classList.remove("is-active");
    ui.loginForm.classList.remove("hidden");
    ui.registerBlock.classList.add("hidden");
    clearMessage();
}

function showRegisterMode() {
    ui.loginModeBtn.classList.remove("is-active");
    ui.registerModeBtn.classList.add("is-active");
    ui.loginForm.classList.add("hidden");
    ui.registerBlock.classList.remove("hidden");
    clearMessage();
}

function showRegisterDoctorTab() {
    ui.registerDoctorTab.classList.add("is-active");
    ui.registerPatientTab.classList.remove("is-active");
    ui.registerDoctorForm.classList.remove("hidden");
    ui.registerPatientForm.classList.add("hidden");
    clearMessage();
}

function showRegisterPatientTab() {
    ui.registerDoctorTab.classList.remove("is-active");
    ui.registerPatientTab.classList.add("is-active");
    ui.registerDoctorForm.classList.add("hidden");
    ui.registerPatientForm.classList.remove("hidden");
    clearMessage();
}

function saveSession(sessionToken, sessionRole, sessionDisplayName) {
    token = sessionToken;
    role = sessionRole;
    displayName = sessionDisplayName;
    localStorage.setItem("authToken", token);
    localStorage.setItem("authRole", role);
    localStorage.setItem("displayName", displayName);
}

function clearSession() {
    token = "";
    role = "";
    displayName = "";
    localStorage.removeItem("authToken");
    localStorage.removeItem("authRole");
    localStorage.removeItem("displayName");
}

function closePanels() {
    ui.panelBackdrop.classList.add("hidden");
    ui.addDiseasePanel.classList.add("hidden");
    ui.editDiseasePanel.classList.add("hidden");
    setInlineMessage(ui.addDiseaseMessage, "");
    setInlineMessage(ui.editDiseaseMessage, "");
}

function openPanel(panelElement) {
    ui.panelBackdrop.classList.remove("hidden");
    panelElement.classList.remove("hidden");
}

function setAddPanelMode(mode) {
    addPanelMode = mode;
    ui.modeExistingBtn.classList.toggle("is-active", mode === "existing");
    ui.modeCustomBtn.classList.toggle("is-active", mode === "custom");
    ui.existingModeBlock.classList.toggle("hidden", mode !== "existing");
    ui.customModeBlock.classList.toggle("hidden", mode !== "custom");
}

function getPatientById(patientId) {
    return doctorPatientsState.find((patient) => patient.id === patientId) || null;
}

function openAddDiseasePanel(patientId) {
    const patient = getPatientById(patientId);
    if (!patient) {
        return;
    }

    ui.addPatientId.value = String(patient.id);
    ui.addPatientLabel.textContent = `${patient.firstName} ${patient.lastName}`;
    ui.addDiseaseForm.reset();
    ui.addPatientId.value = String(patient.id);
    setAddPanelMode("existing");
    setInlineMessage(ui.addDiseaseMessage, "");
    renderDiseasesLibrary();
    openPanel(ui.addDiseasePanel);
}

function openEditDiseasePanel(patientId, diseaseId) {
    const patient = getPatientById(patientId);
    if (!patient) {
        return;
    }
    const disease = patient.diseases.find((item) => item.id === diseaseId);
    if (!disease) {
        return;
    }

    ui.editPatientId.value = String(patientId);
    ui.editDiseaseId.value = String(diseaseId);
    ui.editDiseaseLabel.textContent = `${patient.firstName} ${patient.lastName} • ${disease.name}`;
    ui.editDiseaseName.value = disease.name || "";
    ui.editDiseaseIcd.value = disease.icdCode || "";
    ui.editDiseaseChronic.checked = Boolean(disease.chronic);
    ui.editDiseaseDescription.value = disease.description || "";
    ui.editDiseaseTreatment.value = disease.treatment || "";
    setInlineMessage(ui.editDiseaseMessage, "");
    openPanel(ui.editDiseasePanel);
}

function showAuth() {
    ui.authSection.classList.remove("hidden");
    ui.dashboardSection.classList.add("hidden");
    ui.doctorDashboard.classList.add("hidden");
    ui.patientDashboard.classList.add("hidden");
    ui.logoutBtn.classList.add("hidden");
    ui.welcomeText.textContent = "";
    closePanels();
    disablePixelMode();
}

async function loadDiseasesLibrary() {
    const response = await api("/doctor/diseases/library");
    if (!response.ok) {
        setMessage(t("diseaseLibraryLoadError"), true);
        return false;
    }
    diseasesLibraryState = response.data.diseases || [];
    renderDiseasesLibrary();
    return true;
}

async function refreshDoctorDashboard() {
    const response = await api("/doctor/patients");
    if (!response.ok) {
        clearSession();
        showAuth();
        setMessage(t("doctorPatientsLoadError"), true);
        return false;
    }
    doctorPatientsState = response.data.patients || [];
    renderDoctorPatients(doctorPatientsState);
    await loadDiseasesLibrary();
    return true;
}

async function showDashboard() {
    ui.authSection.classList.add("hidden");
    ui.dashboardSection.classList.remove("hidden");
    ui.logoutBtn.classList.remove("hidden");
    ui.welcomeText.textContent = `${role === "doctor" ? t("welcomeDoctor") : t("welcomePatient")}: ${displayName}`;

    if (role === "doctor") {
        disablePixelMode();
        ui.doctorDashboard.classList.remove("hidden");
        ui.patientDashboard.classList.add("hidden");
        await refreshDoctorDashboard();
    } else if (role === "patient") {
        ui.doctorDashboard.classList.add("hidden");
        ui.patientDashboard.classList.remove("hidden");
        const response = await api("/patient/diseases");
        if (!response.ok) {
            clearSession();
            showAuth();
            setMessage(t("patientDataLoadError"), true);
            return;
        }
        renderPatientDashboard(response.data.patient, true);
    } else {
        clearSession();
        showAuth();
    }
}

async function loadDoctors() {
    const response = await api("/doctors", { requireAuth: false });
    if (!response.ok) {
        setMessage(t("doctorsLoadError"), true);
        return;
    }

    const doctors = response.data.doctors || [];
    if (!doctors.length) {
        ui.doctorSelect.innerHTML = `<option value="">${escapeHtml(t("noDoctors"))}</option>`;
        return;
    }

    ui.doctorSelect.innerHTML = doctors
        .map((doctor) => `<option value="${doctor.id}">${escapeHtml(doctor.fullName)}</option>`)
        .join("");
}

async function onLoginSubmit(event) {
    event.preventDefault();
    clearMessage();

    const formData = new FormData(ui.loginForm);
    const body = {
        role: String(formData.get("role") || ""),
        email: String(formData.get("email") || "").trim(),
        password: String(formData.get("password") || "")
    };

    const response = await api("/login", { method: "POST", body, requireAuth: false });
    if (!response.ok) {
        setMessage(response.data.error || t("loginError"), true);
        return;
    }

    saveSession(response.data.token, response.data.role, response.data.displayName);
    await showDashboard();
}

async function onRegisterDoctorSubmit(event) {
    event.preventDefault();
    clearMessage();

    const formData = new FormData(ui.registerDoctorFormEl);
    const body = {
        fullName: String(formData.get("fullName") || "").trim(),
        email: String(formData.get("email") || "").trim(),
        password: String(formData.get("password") || "")
    };

    const response = await api("/register/doctor", { method: "POST", body, requireAuth: false });
    if (!response.ok) {
        setMessage(response.data.error || t("serverError"), true);
        return;
    }

    setMessage(t("registerDoctorOk"));
    ui.registerDoctorFormEl.reset();
    await loadDoctors();
}

async function onRegisterPatientSubmit(event) {
    event.preventDefault();
    clearMessage();

    const formData = new FormData(ui.registerPatientFormEl);
    const body = {
        firstName: String(formData.get("firstName") || "").trim(),
        lastName: String(formData.get("lastName") || "").trim(),
        doctorId: String(formData.get("doctorId") || "").trim(),
        email: String(formData.get("email") || "").trim(),
        password: String(formData.get("password") || "")
    };

    const response = await api("/register/patient", { method: "POST", body, requireAuth: false });
    if (!response.ok) {
        setMessage(response.data.error || t("serverError"), true);
        return;
    }

    setMessage(t("registerPatientOk"));
    ui.registerPatientFormEl.reset();
}

async function onAddDiseaseSubmit(event) {
    event.preventDefault();
    setInlineMessage(ui.addDiseaseMessage, "");

    const body = {
        patientId: ui.addPatientId.value
    };

    if (addPanelMode === "existing") {
        body.diseaseId = ui.libraryDiseaseId.value;
        if (!body.diseaseId) {
            setInlineMessage(ui.addDiseaseMessage, t("chooseDisease"), true);
            return;
        }
    } else {
        body.name = ui.addDiseaseName.value.trim();
        body.icdCode = ui.addDiseaseIcd.value.trim();
        body.chronic = ui.addDiseaseChronic.checked ? "true" : "false";
        body.description = ui.addDiseaseDescription.value.trim();
        body.treatment = ui.addDiseaseTreatment.value.trim();
        if (!body.name || !body.icdCode || !body.description || !body.treatment) {
            setInlineMessage(ui.addDiseaseMessage, t("addDiseaseError"), true);
            return;
        }
    }

    const response = await api("/doctor/patient/add-disease", { method: "POST", body });
    if (!response.ok) {
        setInlineMessage(ui.addDiseaseMessage, response.data.error || t("addDiseaseError"), true);
        return;
    }

    setInlineMessage(ui.addDiseaseMessage, t("addDiseaseSuccess"), false);
    await refreshDoctorDashboard();
}

async function onEditDiseaseSubmit(event) {
    event.preventDefault();
    setInlineMessage(ui.editDiseaseMessage, "");

    const body = {
        patientId: ui.editPatientId.value,
        diseaseId: ui.editDiseaseId.value,
        name: ui.editDiseaseName.value.trim(),
        icdCode: ui.editDiseaseIcd.value.trim(),
        chronic: ui.editDiseaseChronic.checked ? "true" : "false",
        description: ui.editDiseaseDescription.value.trim(),
        treatment: ui.editDiseaseTreatment.value.trim()
    };

    if (!body.name || !body.icdCode || !body.description || !body.treatment) {
        setInlineMessage(ui.editDiseaseMessage, t("updateDiseaseError"), true);
        return;
    }

    const response = await api("/doctor/patient/update-disease", { method: "POST", body });
    if (!response.ok) {
        setInlineMessage(ui.editDiseaseMessage, response.data.error || t("updateDiseaseError"), true);
        return;
    }

    setInlineMessage(ui.editDiseaseMessage, t("updateDiseaseSuccess"), false);
    await refreshDoctorDashboard();
}

function bindEvents() {
    ui.langRu.addEventListener("click", () => setLanguage("ru"));
    ui.langEn.addEventListener("click", () => setLanguage("en"));

    ui.loginModeBtn.addEventListener("click", showLoginMode);
    ui.registerModeBtn.addEventListener("click", showRegisterMode);
    ui.registerDoctorTab.addEventListener("click", showRegisterDoctorTab);
    ui.registerPatientTab.addEventListener("click", showRegisterPatientTab);

    ui.loginForm.addEventListener("submit", onLoginSubmit);
    ui.registerDoctorFormEl.addEventListener("submit", onRegisterDoctorSubmit);
    ui.registerPatientFormEl.addEventListener("submit", onRegisterPatientSubmit);
    ui.addDiseaseForm.addEventListener("submit", onAddDiseaseSubmit);
    ui.editDiseaseForm.addEventListener("submit", onEditDiseaseSubmit);

    ui.refreshDoctorBtn.addEventListener("click", async () => {
        await refreshDoctorDashboard();
    });

    ui.chronicOnlyToggle.addEventListener("change", () => {
        renderDoctorPatients(doctorPatientsState);
    });

    ui.modeExistingBtn.addEventListener("click", () => setAddPanelMode("existing"));
    ui.modeCustomBtn.addEventListener("click", () => setAddPanelMode("custom"));

    ui.closeAddPanel.addEventListener("click", closePanels);
    ui.closeEditPanel.addEventListener("click", closePanels);
    ui.panelBackdrop.addEventListener("click", closePanels);

    ui.doctorPatients.addEventListener("click", (event) => {
        const addBtn = event.target.closest(".add-disease-btn");
        if (addBtn) {
            const patientId = Number(addBtn.getAttribute("data-patient-id"));
            openAddDiseasePanel(patientId);
            return;
        }

        const editBtn = event.target.closest(".edit-disease-btn");
        if (editBtn) {
            const patientId = Number(editBtn.getAttribute("data-patient-id"));
            const diseaseId = Number(editBtn.getAttribute("data-disease-id"));
            openEditDiseasePanel(patientId, diseaseId);
        }
    });

    ui.logoutBtn.addEventListener("click", () => {
        clearSession();
        showAuth();
        setMessage(t("authRequired"), true);
    });

    ui.result.addEventListener("click", (event) => {
        const card = event.target.closest(".disease");
        if (!card) {
            return;
        }

        const isOpen = card.classList.contains("is-open");
        ui.result.querySelectorAll(".disease.is-open").forEach((element) => {
            element.classList.remove("is-open");
        });
        if (!isOpen) {
            card.classList.add("is-open");
        }
    });
}

async function init() {
    bindEvents();
    setLanguage(language);
    showLoginMode();
    showRegisterDoctorTab();
    await loadDoctors();

    if (token && role) {
        await showDashboard();
    } else {
        showAuth();
    }
}

init();
