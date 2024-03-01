const connectors = {
    x4: "X4",
    x6: "X6",
}
const configurableSensors = document.createElement("ul");
const contentElement = document.getElementById("content");

async function fetchJson(url) {
    const json = await fetch(url)
        .then(response => response.json());
    
    return json != null ? json : {};
}

function createAddSensorForm(sensorType, options) {
    const wrapper = document.createElement("div");
    wrapper.append(options.name);
    const form = createElement(
        "form",
        {action: "/api/sensors/add", method: "post"},
        createHiddenInput("sensorType", sensorType),
        createRadioButtons("connectionType", options.connections, sensorType, true),
        createRadioButtons("connection", connectors, sensorType, true),
        createSubmitButton("add")
    )
    wrapper.append(form);

    return wrapper;
}

const createSubmitButton = (value) => createInput({type: "submit", value: value});

function createRadioButtons(name, options, idPrefix = "", required = false) {
    const wrapper = document.createElement("div");
    
    for (const opt in options) {
        if (!Object.hasOwnProperty.call(options, opt)) {
            continue;
        }

        const buttonId = idPrefix ? `${idPrefix}_${name}_${opt}` : `${name}_${opt}`;

        wrapper.appendChild(createInput({
            type: "radio",
            name: name,
            value: opt,
            id: buttonId,
            required: required,
            selected: options.length == 1,
        }));
        wrapper.appendChild(createElement("label", {for: buttonId}, options[opt]));
    }

    return wrapper;
}

function createElement(type, attributes, ...children) {
    const element = document.createElement(type);
    for (const attribute in attributes) {
        if (!Object.hasOwnProperty.call(attributes, attribute)) {
            continue;
        }

        element.setAttribute(attribute, attributes[attribute])
    }

    for (const child of children) {
        element.append(child);
    }
    
    return element;
}

const createInput = (attributes) => createElement("input", attributes);

const createHiddenInput = (name, value) => createInput({
        type: "hidden",
        name: name,
        value: value,
    });


sensorsScreen();
