import AbstractView from "../AbstractView.js";

class Upload extends AbstractView {
    constructor(params) {
        super(params);
    }

    async sendFile(file) {
        if (!file) return;

        const formData = new FormData();
        formData.append("data", file);

        await fetch("/api/filesystem/upload",
            {
                method: "POST",
                body: formData,
            }
        );
    }

    bindHandlers() {
        const sform = document.querySelector("#uploadSingleForm");
        const that = this;
        sform.addEventListener("submit", (event) => {
            event.preventDefault();
            const picker = document.getElementById("singleFilePicker");
            that.sendFile(picker.files[0]);
        });
        const mform = document.querySelector("#uploadMultipleForm");
        mform.addEventListener("submit", async (event) => {
            event.preventDefault();
            const picker = document.getElementById("multipleFilePicker");
            
            const progress = document.getElementById("uploadProgress");
            progress.setAttribute("max", picker.files.length);
            progress.setAttribute("value", 0);

            for (let i = 0; i < picker.files.length; i++) {
                const file = picker.files[i];
                await that.sendFile(file);
                progress.setAttribute("value", i+1);
                progress.innerHTML = `${(i/picker.files.length) * 100}%`;
            }

        });
    }

    async getHtml() {
        return `
            <form id="uploadSingleForm">
                <input type="file" id="singleFilePicker" name="file"/>
                <button type="submit">Submit</button>
            </form>
            <form id="uploadMultipleForm">
                <input type="file" id="multipleFilePicker" name="fileList" webkitdirectory multiple />
                <button type="submit">Submit</button>
            </form>
            <progress id="uploadProgress" max="100" value="0">0%</progress>
        `;
    }
}

export default Upload;
