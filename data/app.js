const contentElement = document.getElementById("content");
const navigationElement = document.getElementById("nav");

async function fetchJson(url) {
    const json = await fetch(url)
        .then(response => response.json());
    
    return json != null ? json : {};
}


const createNavigationLink = (label, callback) => {
    const link = createElementWithAttributes("a", {href: "#"}, label);
    link.onclick = callback;
    return link;
}

const buildNavigation = () => {
    navigationElement.append(createElementWithAttributes("ui", {}, 
        createElementWithAttributes("li", {}, createNavigationLink("Sensors", sensorsScreen)),
        createElementWithAttributes("li", {}, createNavigationLink("Calculators", calculatorScreen)),
    ))
}

buildNavigation();
sensorsScreen();
