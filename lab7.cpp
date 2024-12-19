class Solution {
public:
    int scheduleCourse(vector<vector<int>>& courses) {
        sort(courses.begin(), courses.end(), [](const vector<int>& a, const vector<int>& b) {
            return a[1] < b[1];
        });

        priority_queue<int> maxHeap;
        int totalTime = 0;

        for (const auto& course : courses) {
            int duration = course[0];
            int lastDay = course[1];

            totalTime += duration;
            maxHeap.push(duration);

            if (totalTime > lastDay) {
                totalTime -= maxHeap.top();
                maxHeap.pop();
            }
        }

        return maxHeap.size();
    }
};


// отчет - https://docs.google.com/document/d/1uFVW42I14Nkql6w0Nb0UzosDo8nPLew1D_o1S937CRc/edit?tab=t.0
