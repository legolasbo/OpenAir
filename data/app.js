const contentElement = document.getElementById("content");
const navigationElement = document.getElementById("nav");

async function fetchJson(url) {
    const json = await fetch(url)
        .then(response => response.json());
    
    return json != null ? json : {};
}


const createNavigationLink = (label, callback) => {
    const link = createElement("a", {href: "#"}, label);
    link.onclick = callback;
    return link;
}

const buildNavigation = () => {
    navigationElement.append(createElement("ui", {}, 
        createElement("li", {}, createNavigationLink("Sensors", sensorsScreen)),
        createElement("li", {}, createNavigationLink("Calculators", calculatorScreen)),
    ))
}

buildNavigation();
sensorsScreen();
